use std::env;

fn main() {
    let rustc_version = rustc_version::version().unwrap();
    assert_eq!(rustc_version.major, 1);
    // This probably has a minor gap in correctness for some half-release-cycles of nightlies, but
    // that's OK because we'll remove that ASAP anyway (after bumping MSRV).
    if rustc_version.minor >= 100 {
        println!("cargo:rustc-cfg=rustc_has_never");
    }
    println!("cargo::rustc-check-cfg=cfg(rustc_has_never)");

    println!("cargo:rerun-if-env-changed=RIOTBUILD_CONFIG_HEADER_C");
    let riotbuildh = env::var("RIOTBUILD_CONFIG_HEADER_C")
        .expect("riotbuild.h is expected to be indicated in the RIOTBUILD_CONFIG_HEADER_C environment variable, or another source of enabled modules provided.");
    println!("cargo:rerun-if-changed={riotbuildh}");

    let mut defines = std::collections::HashMap::new();

    use std::io::BufRead;
    for line in std::io::BufReader::new(
        std::fs::File::open(riotbuildh)
            .expect("Failed to read riotbuild.h (RIOTBUILD_CONFIG_HEADER_C)"),
    )
    .lines()
    {
        let line = line.expect("Error reading line from riotbuild.h (RIOTBUILD_CONFIG_HEADER_C)");
        if let Some(name) = line.strip_prefix("#undef ") {
            defines.remove(name.trim());
        }
        if let Some((name, val)) = line
            .strip_prefix("#define ")
            .and_then(|nv| nv.split_once(" "))
        {
            defines.insert(name.trim().to_owned(), val.trim().to_owned());
        }
    }

    const BOOLEAN_FLAGS: &[&str] = &[
        // This decides whether or not some fields are populated ... and unlike with other
        // structs, the zeroed default is not a good solution here. (It'd kind of work, but
        // it'd produce incorrect debug output).
        "CONFIG_AUTO_INIT_ENABLE_DEBUG",
    ];

    for marker in BOOLEAN_FLAGS {
        println!(
            "cargo::rustc-check-cfg=cfg(marker_{})",
            marker.to_lowercase()
        );
    }

    for (def, val) in defines {
        if val != "1" {
            // So far, only processing boolean flags
            continue;
        }
        if let Some(module) = def.strip_prefix("MODULE_") {
            // Some modules like cmsis-dsp_StatisticsFunctions have funny characters
            println!(
                "cargo:rustc-cfg=riot_module_{}",
                module.to_lowercase().replace("-", "_")
            );
        }
        if def == "DEVELHELP" {
            println!("cargo:rustc-cfg=riot_develhelp");
        }
        if BOOLEAN_FLAGS.contains(&def.as_str()) {
            println!("cargo:rustc-cfg=marker_{}", def.to_lowercase());
        }
    }

    println!("cargo::rustc-check-cfg=cfg(riot_develhelp)");

    // FIXME: This list is currently maintained manually;
    let known_modules = &[
        "auto_init",
        "auto_init_random",
        "bluetil_ad",
        "core_msg",
        "gcoap",
        "gnrc",
        "gnrc_icmpv6",
        "gnrc_ipv6_nib",
        "gnrc_netapi_callbacks",
        "gnrc_nettype_ccn",
        "gnrc_nettype_custom",
        "gnrc_nettype_gomach",
        "gnrc_nettype_icmpv6",
        "gnrc_nettype_ipv6",
        "gnrc_nettype_ipv6_ext",
        "gnrc_nettype_lwmac",
        "gnrc_nettype_ndn",
        "gnrc_nettype_sixlowpan",
        "gnrc_nettype_tcp",
        "gnrc_nettype_udp",
        "gnrc_pktbuf",
        "gnrc_udp",
        "ipv6",
        "microbit",
        "nimble_host",
        "periph_adc",
        "periph_dac",
        "periph_gpio",
        "periph_i2c",
        "periph_spi",
        "periph_uart",
        "periph_uart_collision",
        "periph_uart_hw_fc",
        "periph_uart_modecfg",
        "periph_uart_nonblocking",
        "periph_uart_reconfigure",
        "periph_uart_rxstart_irq",
        "periph_uart_tx_ondemand",
        "prng_shaxprng",
        "pthread",
        "random",
        "saul",
        "shell",
        "sock",
        "sock_aux_local",
        "sock_tcp",
        "sock_udp",
        "tiny_strerror",
        "tiny_strerror_minimal",
        "udp",
        "vfs",
        "ws281x",
        "ztimer",
        "ztimer_msec",
        "ztimer_periodic",
        "ztimer_sec",
        "ztimer_usec",
    ];

    for module in known_modules {
        println!("cargo::rustc-check-cfg=cfg(riot_module_{})", module);
    }

    // As a means of last resort, we emulate cfg(accessible(::path::to::thing)), as a
    // workaround for https://github.com/rust-lang/rust/issues/64797
    //
    // This is sometimes necessary when some C API compatible change (eg. renaming a field with a
    // deprecated union while introducing a proper accessor, as done in
    // https://github.com/RIOT-OS/RIOT/pull/20900) leads to changes in bindgen and c2rust outputs.
    //
    // This is similar to the markers that were previously used in riot-sys. We access files
    // directly to avoid going through `links=` (see also
    // <https://github.com/RIOT-OS/rust-riot-sys/pull/38>). This
    //
    // * limits the impact of source changes (this way, only changs to relevant headerst trigger
    //   a rebuild of riot-wrappers), and
    // * removes the need for lock-stepping riot-sys and riot-wrappers.
    //
    // The downside of the tighter coupling with the paths in RIOT is reduced by keeping things
    // local and the stabiity structure: All these access checks can be removed once riot-wrappers
    // stops supporting a RIOT version that has the symbol unconditionally, without any concern for
    // compatibility between crates (as the cfgs are just internal).

    let riotbase: std::path::PathBuf = env::var("RIOTBASE")
        .expect("No RIOTBASE set, can not inspect source files for symbol presence.")
        .into();
    println!("cargo:rerun-if-env-changed=RIOTBASE");

    // FIXME: We're trying to get rid of that dependency in
    // <https://github.com/RIOT-OS/rust-riot-sys/pull/38>, but for this one, there are no good
    // alternatives.
    let bindgen_output_file = std::env::var("DEP_RIOT_SYS_BINDGEN_OUTPUT_FILE").unwrap();
    let emulate_accessible = [
        // It's a static inline function and riot-sys currently only gives the file for the bindgen
        // output, not the c2rust output. Using coap_build_udp_hdr presence as a stand-in.
        //
        // Remove this after a release including coap_pkt_set_code
        // <https://github.com/RIOT-OS/riot/issues/20900> has been published.
        (
            &"inline_coap_pkt_set_code",
            &"sys/include/net/nanocoap.h",
            &"coap_pkt_set_code",
        ),
        (
            &"spi_clk_t_SPI_CLK_100KHZ",
            &bindgen_output_file.as_str(),
            &"spi_clk_t_SPI_CLK_100KHZ",
        ),
        (
            &"sock_ep_t_member_ip",
            &"sys/include/net/sock.h",
            &"@deprecated use @ref sock_ip_ep_t::addr::v6 instead",
        ),
    ];

    for (rust_name, header_file, header_search_string) in emulate_accessible {
        let header_file = riotbase.join(header_file);
        println!("cargo:rerun-if-changed={}", header_file.display());
        let header_code =
            std::fs::read_to_string(&header_file).expect("Failed to read header file");
        println!("cargo:rustc-check-cfg=cfg(accessible_riot_sys_{rust_name})");
        if header_code.contains(header_search_string) {
            println!("cargo:rustc-cfg=accessible_riot_sys_{rust_name}");
        }
    }
}
