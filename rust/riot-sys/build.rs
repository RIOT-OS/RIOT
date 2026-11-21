extern crate bindgen;
extern crate shlex;

use bindgen::builder;
use std::env;
use std::fmt::Write;
use std::path::PathBuf;

fn main() {
    let cc;
    let mut cflags;

    #[cfg(not(feature = "riot-rs"))]
    if env::var("BUILDING_RIOT_RS").is_ok() {
        println!("");
        println!("ERROR: riot-sys seems to be built for RIOT-rs (BUILDING_RIOT_RS is set). Please enable the 'riot-rs' feature.");
        println!(
            "To do this, make the main application crate depend on `riot-sys` with feature `riot-rs`."
        );
        println!("");
        std::process::exit(1);
    }

    #[cfg(not(feature = "riot-rs"))]
    let compile_commands_json = "RIOT_COMPILE_COMMANDS_JSON";
    #[cfg(feature = "riot-rs")]
    let compile_commands_json = "DEP_RIOT_BUILD_COMPILE_COMMANDS_JSON";

    println!("cargo:rerun-if-env-changed=BUILDING_RIOT_RS");
    println!("cargo:rerun-if-env-changed=RIOT_CC");
    println!("cargo:rerun-if-env-changed=RIOT_CFLAGS");
    println!("cargo:rerun-if-env-changed={}", &compile_commands_json);

    if let Ok(commands_json) = env::var(compile_commands_json) {
        println!("cargo:rerun-if-changed={}", commands_json);
        let commands_file = std::fs::File::open(&commands_json)
            .expect(&format!("Failed to open {}", &commands_json));

        #[derive(Debug, serde::Deserialize)]
        struct Entry {
            arguments: Vec<String>,
        }
        let parsed: Vec<Entry> = serde_json::from_reader(commands_file)
            .expect(&format!("Failed to parse {}", &compile_commands_json));

        // We need to find a consensus list -- otherwise single modules like stdio_uart that
        // defines anything odd for its own purpose can throw things off. (It's not like the actual
        // ABI compatibility should suffer from them, for any flags like enum packing need to be
        // the same systemwide anyway for things to to go very wrong) -- but at any rate, finding
        // some consensus is to some extent necessary here).
        //
        // This is relatively brittle, but still better than the previous approach of just taking
        // the first entry.
        //
        // A good long-term solution might be to take CFLAGS as the build system produces them, but
        // pass them through the LLVMization process of create_compile_commands without actually
        // turning them into compile commands.
        let mut consensus_cc: Option<&str> = None;
        let mut consensus_cflag_groups: Option<Vec<Vec<&str>>> = None;
        for entry in parsed.iter() {
            if let Some(consensus_cc) = consensus_cc.as_ref() {
                assert!(consensus_cc == &entry.arguments[0])
            } else {
                consensus_cc = Some(&entry.arguments[0]);
            }
            let arg_iter = entry.arguments[1..]
                .iter()
                .map(|s| s.as_str())
                // Anything after -c is not CFLAGS but concrete input/output stuff.
                .take_while(|&s| s != "-c" && s != "-MQ");
            // Heuristically grouping them to drop different arguments as whole group
            let mut cflag_groups = vec![];
            for mut arg in arg_iter {
                if arg.starts_with("-I") {
                    // -I arguments are given inconsistently with and without trailing slashes;
                    // removing them keeps them from being pruned from the consensus set
                    arg = arg.trim_end_matches('/');
                }
                if arg.starts_with('-') {
                    cflag_groups.push(vec![arg]);
                } else {
                    cflag_groups
                        .last_mut()
                        .expect("CFLAG options all start with a dash")
                        .push(arg);
                }
            }
            if let Some(consensus_cflag_groups) = consensus_cflag_groups.as_mut() {
                if &cflag_groups != consensus_cflag_groups {
                    // consensus is in a good ordering, so we'll just strip it down
                    *consensus_cflag_groups = consensus_cflag_groups
                        .drain(..)
                        .filter(|i| {
                            let mut keep = cflag_groups.contains(i);
                            // USEMODULE_INCLUDES are sometimes not in all of the entries; see note
                            // on brittleness above.
                            keep |= i[0].starts_with("-I");
                            // Left as multiple lines to ease hooking in with debug statements when
                            // something goes wrong again...
                            keep
                        })
                        .collect();
                    // Hot-fixing the merging algorithm to even work when an (always to be kept) -I
                    // is not in the initial set
                    for group in cflag_groups.drain(..) {
                        if group[0].starts_with("-I") {
                            if !consensus_cflag_groups.contains(&group) {
                                consensus_cflag_groups.push(group);
                            }
                        }
                    }
                }
            } else {
                consensus_cflag_groups = Some(cflag_groups);
            }
        }
        cc = consensus_cc
            .expect("Entries are present in compile_commands.json")
            .to_string();
        cflags = shlex::try_join(consensus_cflag_groups.unwrap().iter().flatten().map(|s| *s))
            .expect("Input is not expected to contain NUL characters");

        let usemodule = {
            #[cfg(not(feature = "riot-rs"))]
            {
                println!("cargo:rerun-if-env-changed=RIOT_USEMODULE");
                // We tolerate the absence. Older versions of riot-wrappers would then fail to
                // enable modules, but newer versions just work without it (and would need a dummy
                // variable passed in otherwise). On the long run, this is going away anyway.
                env::var("RIOT_USEMODULE").unwrap_or_default()
            }
            #[cfg(feature = "riot-rs")]
            {
                println!("cargo:rerun-if-env-changed=DEP_RIOT_BUILD_DIR");
                let riot_builddir =
                    env::var("DEP_RIOT_BUILD_DIR").expect("DEP_RIOT_BUILD_DIR unset?");
                get_riot_var(&riot_builddir, "USEMODULE")
            }
        };

        for m in usemodule.split(" ") {
            // Hack around https://github.com/RIOT-OS/RIOT/pull/16129#issuecomment-805810090
            write!(
                cflags,
                " -DMODULE_{}",
                m.to_uppercase()
                    // avoid producing MODULE_BOARDS_COMMON_SAMDX1-ARDUINO-BOOTLOADER
                    .replace('-', "_")
            )
            .unwrap();
        }
    } else {
        cc = env::var("RIOT_CC")
            .expect("Please pass in RIOT_CC; see README.md for details.")
            .clone();
        cflags = env::var("RIOT_CFLAGS")
            .expect("Please pass in RIOT_CFLAGS; see README.md for details.");
    }

    // pass CC and CFLAGS to dependees
    // this requires a `links = "riot-sys"` directive in Cargo.toml.
    // Dependees can then access these as DEP_RIOT_SYS_CC and DEP_RIOT_SYS_CFLAGS.
    println!("cargo:CC={}", &cc);
    println!("cargo:CFLAGS={}", &cflags);

    let cflags = shlex::split(&cflags).expect("Odd shell escaping in RIOT_CFLAGS");
    let cflags: Vec<String> = cflags
        .into_iter()
        .filter(|x| {
            match x.as_ref() {
                // These will be in riotbuild.h as well, and better there because bindgen emits
                // consts for data from files but not from defines (?)
                x if x.starts_with("-D") => false,
                // Don't pollute the riot-sys source directory -- cargo is run unconditionally
                // in the Makefiles, and this script tracks on its own which files to depend on
                // for rebuilding.
                "-MD" => false,
                // accept all others
                _ => true,
            }
        })
        .collect();

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());

    println!("cargo:rerun-if-changed=riot-bindgen.h");
    println!("cargo:rerun-if-changed=riot-headers.h");

    // Function-like and initializer macros that are made available as functions.
    //
    // They are emitted as `static inline` functions into the generated header; from there, they
    // are picked up by bindgen's static function wrapping like any other `static inline` function
    // of RIOT.
    //
    // Entries marked "is const" need to be scalar: Rather than a function, a typed `static const`
    // is emitted, which bindgen evaluates; it is exposed as a `const fn macro_NAME()`.
    //
    // (`MUTEX_INIT` is not in here, but implemented manually in `src/manual.rs`, as it is needed
    // in const context, but is a struct).
    //
    // Fields: macro name, return type, arguments, is const, fallback value
    let macro_functions = [
        ("SOCK_IPV4_EP_ANY", "sock_udp_ep_t", None, false, None),
        ("SOCK_IPV6_EP_ANY", "sock_udp_ep_t", None, false, None),
        // neither C2Rust nor bindgen understand the cast without help
        ("STATUS_NOT_FOUND", "thread_status_t", None, true, None),
        // If any board is ever added that works completely differently, this'll have to go behind
        // a feature-gate
        (
            "GPIO_PIN",
            "gpio_t",
            Some("unsigned port, unsigned pin"),
            // would be nice to have them const, but on boards like samd21-xpro that'd require
            // several nightly features (const_ptr_offset, const_mut_refs).
            false,
            None,
        ),
        // These are bound to the signature already in periph_init.
        ("I2C_DEV", "i2c_t", Some("unsigned num"), false, None),
        ("SPI_DEV", "spi_t", Some("unsigned num"), false, None),
        // No good source on why this sould have a fixed signature, but at this point it's a
        // pattern.
        ("UART_DEV", "uart_t", Some("unsigned num"), false, None),
        ("PWM_DEV", "pwm_t", Some("unsigned num"), false, None),
        ("ADC_LINE", "adc_t", Some("unsigned num"), false, None),
        // RIOT's timer type is tim_t; timer_t would be POSIX' (which is a pointer on native).
        ("TIMER_DEV", "tim_t", Some("unsigned num"), false, None),
        ("QDEC_DEV", "qdec_t", Some("unsigned num"), false, None),
        ("DAC_LINE", "dac_t", Some("unsigned num"), false, None),
    ];
    let mut macro_functions: Vec<_> = macro_functions
        .iter()
        .map(
            |(macro_name, return_type, args, is_const, fallback_value)| {
                (
                    macro_name.to_string(),
                    *return_type,
                    *args,
                    *is_const,
                    *fallback_value,
                )
            },
        )
        .collect();
    for i in 0..8 {
        macro_functions.push((format!("LED{}_ON", i), "void", None, false, None));
        macro_functions.push((format!("LED{}_OFF", i), "void", None, false, None));
        macro_functions.push((format!("LED{}_TOGGLE", i), "void", None, false, None));
        macro_functions.push((
            format!("LED{}_IS_PRESENT", i),
            "int",
            Some("defined"),
            true,
            Some("-1"),
        ));
    }

    // Constants that bindgen can not express from the macro alone (because they contain casts,
    // or because they are enum members that bindgen prefixes with the enum name). They are made
    // available under their C name with their C type. On the C side, they go through a `static
    // const long long` (because bindgen only evaluates initializers of plain integer types, not
    // of enums), and are cast to their type in Rust.
    //
    // Fields: name, C type, preprocessor condition (default: `defined(name)`)
    let typed_constants: &[(&str, &str, Option<&str>)] = &[
        ("GPIO_UNDEF", "gpio_t", None),
        ("STATUS_NOT_FOUND", "thread_status_t", None),
        // Enum members of RIOT's af.h, which also provides `#define AF_INET AF_INET` etc.
        ("AF_UNSPEC", "unix_af_t", None),
        ("AF_UNIX", "unix_af_t", None),
        ("AF_PACKET", "unix_af_t", None),
        ("AF_INET", "unix_af_t", None),
        ("AF_INET6", "unix_af_t", None),
        (
            "SOCK_ASYNC_MSG_RECV",
            "sock_async_flags_t",
            Some("defined(MODULE_SOCK_ASYNC)"),
        ),
    ];
    const CONST_PREFIX: &str = "riot_sys_const_";

    let mut header = String::new();
    writeln!(
        header,
        "/* Generated by riot-sys' build.rs -- do not edit */\n\
         #include \"{}\"\n",
        manifest_dir.join("riot-bindgen.h").display()
    )
    .unwrap();

    const MACROCONST_PREFIX: &str = "riot_sys_macroconst_";
    for (macro_name, return_type, mut args, is_const, fallback_value) in macro_functions.iter() {
        let expression = match args {
            None => macro_name.to_string(),
            Some("void") => format!("{macro_name}()"),
            // Special handling for macros that are not used as values, but are more of an ifdef
            // guard.
            Some("defined") => {
                args = None;
                // No need to check further: the function is already inside an ifdef
                "1".to_string()
            }
            Some(args) => format!(
                "{macro_name}({})",
                args.split(", ")
                    .map(|s| &s[s.find(" ").expect("Non-void args need to have names")..])
                    .collect::<Vec<&str>>()
                    .join(", ")
            ),
        };
        let args = args.unwrap_or("void");

        // The ifdef guards make errors easier to spot: A "cannot find function
        // `macro_SOCK_IPV6_EP_ANY` in crate `riot_sys`" can lead one to check whether
        // SOCK_IPV6_EP_ANY is really defined.
        writeln!(header, "#ifdef {macro_name}").unwrap();
        if *is_const {
            assert!(args == "void", "Const macros can not take arguments");
            writeln!(
                header,
                "static const long long {MACROCONST_PREFIX}{macro_name} = (long long)({expression});"
            )
        } else if *return_type == "void" {
            // in C, assigning and returning void is special
            writeln!(
                header,
                "static inline void macro_{macro_name}({args}) {{\n    {expression};\n}}"
            )
        } else {
            writeln!(
                header,
                "static inline {return_type} macro_{macro_name}({args}) {{\n    \
                 {return_type} result = {expression};\n    return result;\n}}"
            )
        }
        .unwrap();
        if let Some(fallback_value) = fallback_value {
            if *is_const {
                writeln!(
                    header,
                    "#else\nstatic const long long {MACROCONST_PREFIX}{macro_name} = {fallback_value};"
                )
            } else {
                writeln!(
                header,
                "#else\nstatic inline {return_type} macro_{macro_name}({args}) {{\n    \
                 return {fallback_value};\n}}"
                )
            }
            .unwrap();
        }
        writeln!(header, "#endif\n").unwrap();
    }

    for (name, _ctype, condition) in typed_constants {
        let condition = condition
            .map(|c| c.to_string())
            .unwrap_or_else(|| format!("defined({name})"));
        writeln!(
            header,
            "#if {condition}\nstatic const long long {CONST_PREFIX}{name} = (long long)({name});\n#endif\n"
        )
        .unwrap();
    }

    let header_path = out_path.join("riot-sys.h");
    std::fs::write(&header_path, header).expect("Failed to write riot-sys.h");

    // Wrappers for static (inline) functions
    //
    // bindgen produces a C file that contains non-static wrappers around all static functions;
    // the Rust declarations link to these wrappers. That C file needs to be built with the same
    // compiler and flags as the rest of RIOT and linked into the application; RIOT's build system
    // does that, and tells us where to put the file through RIOT_SYS_STATIC_FNS_C.
    //
    // Without that variable (eg. when just running `cargo check` or `cargo doc`), the file is
    // placed in OUT_DIR, and linking will fail unless something else picks it up from there.
    println!("cargo:rerun-if-env-changed=RIOT_SYS_STATIC_FNS_C");
    let static_fns_c = env::var("RIOT_SYS_STATIC_FNS_C")
        .map(PathBuf::from)
        .unwrap_or_else(|_| out_path.join("static_fns.c"));
    assert!(
        static_fns_c.extension().is_some_and(|e| e == "c"),
        "RIOT_SYS_STATIC_FNS_C needs to end in .c"
    );
    // bindgen does not write the file if there is nothing to wrap; don't let stale versions linger.
    let _ = std::fs::remove_file(&static_fns_c);

    let mut builder = builder()
        .header(header_path.to_str().expect("OUT_DIR is UTF-8"))
        .clang_args(&cflags)
        // Enables workarounds in riot-bindgen.h that are only needed (and only correct) when
        // parsing the headers with libclang, as opposed to when compiling the static fn wrappers.
        .clang_arg("-DRIOT_SYS_BINDGEN")
        .wrap_static_fns(true)
        .wrap_static_fns_path(static_fns_c.with_extension(""));
    for (name, _, _) in typed_constants {
        // If bindgen understands the macro on its own, it'd produce a clashing definition.
        builder = builder.blocklist_item(name);
    }
    let bindings = builder
        .use_core()
        .ctypes_prefix("core::ffi")
        // This is the defensive choice in presence of odd targets, as it forces us to be explicit
        // more often: Bindgen would err with "Target platform requires `--no-size_t-is-usize`." in
        // some cases, even though we have not encountered them yet.
        .size_t_is_usize(false)
        .impl_debug(true)
        // Structs listed here are Packed and thus need impl_debug, but also contain non-Copy
        // members.
        //
        // This is a workaround for <https://github.com/rust-lang/rust-bindgen/issues/2221>; once
        // that is fixed and our bindgen is updated, these can just go away again.
        //
        // If you see any errors like
        //
        // ```
        // error: reference to packed field is unaligned
        //      --> .../out/bindings.rs:79797:13
        //       |
        // 79797 |             self.opcode, self.length, self.data
        //       |             ^^^^^^^^^^^
        //       |
        //       = note: `#[deny(unaligned_references)]` on by default
        //       = warning: this was previously accepted by the compiler but is being phased out; it will become a hard error in a future release!
        //       = note: for more information, see issue #82523 <https://github.com/rust-lang/rust/issues/82523>
        //       = note: fields of packed structs are not properly aligned, and creating a misaligned reference is undefined behavior (even if that reference is never dereferenced)
        //       = help: copy the field contents to a local variable, or replace the reference with a raw pointer and use `read_unaligned`/`write_unaligned` (loads and stores via `*p` must be properly aligned even when using raw pointers)
        //       = note: this error originates in the macro `$crate::format_args` (in Nightly builds, run with -Z macro-backtrace for more info)
        // ```
        //
        // please add the offending struct in here; if existing code depends on the Debug
        // implementation, you may add a Debug implementation (that possibly is just a dummy, for
        // in these cases it *is* hard to implement showing all details) to this crate for the
        // duration of these workarounds.
        .no_debug("ble_hci_cmd")
        .no_debug("ble_hci_ev_command_complete")
        .no_debug("ble_hci_ev_le_subev_big_complete")
        .no_debug("ble_hci_ev_le_subev_big_sync_established")
        .no_debug("ble_hci_ev_le_subev_create_big_complete")
        .no_debug("ble_hci_ev_le_subev_cs_subevent_result")
        .no_debug("ble_hci_ev_le_subev_cs_subevent_result_continue")
        .no_debug("ble_hci_ev_le_subev_periodic_adv_rpt")
        .no_debug("ble_hci_iso")
        .no_debug("ble_hci_iso_data")
        .no_debug("ble_hci_le_big_create_sync_cp")
        .no_debug("ble_hci_le_cs_test_cp")
        .no_debug("ble_hci_le_set_cig_params_cp")
        .no_debug("ble_hci_le_set_cig_params_rp")
        .no_debug("ble_hci_le_set_cig_params_test_cp")
        .no_debug("ble_hci_le_set_cig_params_test_rp")
        .no_debug("ble_hci_le_setup_iso_data_path_cp")
        .no_debug("ext_adv_report")
        .derive_default(true)
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate()
        .expect("Unable to generate bindings");

    if !static_fns_c.exists() {
        // Keep the C side simple: there is always a file to build.
        std::fs::write(
            &static_fns_c,
            "/* No static functions were found by riot-sys */\n\
             typedef int riot_sys_no_static_fns;\n",
        )
        .expect("Failed to write static fns stub");
    }
    // Backdate the file: If it is removed, the build script is rerun (and recreates it), but its
    // own creation should not cause reruns. (RIOT rebuilds it unconditionally anyway).
    std::fs::File::options()
        .write(true)
        .open(&static_fns_c)
        .and_then(|f| f.set_modified(std::time::UNIX_EPOCH + std::time::Duration::from_secs(1)))
        .expect("Failed to backdate static fns file");
    println!("cargo:rerun-if-changed={}", static_fns_c.display());

    let mut bindgen_output = Vec::<u8>::new();
    bindings
        .write(Box::new(&mut bindgen_output))
        .expect("String writing never fails");
    let bindgen_outfilename = out_path.join("bindings.rs");
    std::fs::write(&bindgen_outfilename, &bindgen_output).expect("Couldn't write bindings!");
    // Kept for inspection for markers; see there
    let bindgen_output = std::str::from_utf8(&bindgen_output).expect("Rust source code is UTF-8");

    // Const accessors for the scalar macros, and typed constants
    let mut macro_consts = String::new();
    let rust_type = |ctype: &str| match ctype {
        "int" => "core::ffi::c_int".to_string(),
        other => other.to_string(),
    };
    let evaluated = |name: &str| {
        let found = bindgen_output.contains(&format!("pub const {name}: "));
        if !found && bindgen_output.contains(&format!("pub static {name}: ")) {
            panic!("bindgen could not evaluate {name}");
        }
        found
    };
    for (macro_name, return_type, _, is_const, _) in macro_functions.iter() {
        if *is_const && evaluated(&format!("{MACROCONST_PREFIX}{macro_name}")) {
            let rust_type = rust_type(return_type);
            writeln!(
                macro_consts,
                "pub const unsafe fn macro_{macro_name}() -> {rust_type} \
                 {{ {MACROCONST_PREFIX}{macro_name} as {rust_type} }}"
            )
            .unwrap();
        }
    }
    for (name, ctype, _) in typed_constants {
        if evaluated(&format!("{CONST_PREFIX}{name}")) {
            let rust_type = rust_type(ctype);
            writeln!(
                macro_consts,
                "pub const {name}: {rust_type} = {CONST_PREFIX}{name} as {rust_type};"
            )
            .unwrap();
        }
    }
    std::fs::write(out_path.join("macro_consts.rs"), macro_consts)
        .expect("Failed to write macro_consts.rs");

    enum MarkerCondition {
        /// This has been around for long enough that no actual check is performed any more, the
        /// marker is just always set. Markers are set to that when the oldest supported RIOT
        /// version has the new behavior; users of riot-sys may stop checking for the marker when
        /// they depend on a riot-sys version that has it on Always.
        Always,
        /// A marker that has been around for some time during while preparing some PRs, but never
        /// was merged, and the PR was abandoned.
        ///
        /// This is equivalent to not having the marker in the first place, except that their
        /// presence serves as a reminder to not reuse that marker name.
        Never,
        /// A marker that is set if its name is found in the bindgen output. Shorthand for
        /// Text(name).
        NameInCode,
    }

    use MarkerCondition::*;

    let markers = [
        // See https://github.com/RIOT-OS/RIOT/pull/17569, available after 2022.01
        (Always, "phydat_unit_t"),
        // See https://github.com/RIOT-OS/RIOT/pull/17660, available after 2022.01
        (Always, "vfs_iterate_mount_dirs"),
        // See https://github.com/RIOT-OS/RIOT/pull/17758 retrofitting it for the change in
        // https://github.com/RIOT-OS/RIOT/pull/17351, available in 2022.04
        (Always, "ztimer_periodic_callback_t"),
        // Experimental markers
        //
        // These are not merged in RIOT yet, but promising candidates; if there are any substantial
        // changes to them, their marker name will be bumped, but it is expected that they will be
        // moved up and get an "available after" release once merged.

        // See https://github.com/RIOT-OS/RIOT/pull/17544
        (Never, "coap_build_pkt_t"),
        (Never, "gcoap_resource_t"),
        // See https://github.com/RIOT-OS/RIOT/pull/17957, available TBD
        (NameInCode, "coap_request_ctx_t"),
    ];
    for (needle, name) in markers {
        let found = match needle {
            NameInCode => bindgen_output.contains(name),
            Always => true,
            Never => false,
        };
        if found {
            println!("cargo:MARKER_{}=1", name);
        }
    }

    // let downstream crates know we're building for riot-rs
    #[cfg(feature = "riot-rs")]
    println!("cargo:MARKER_riot_rs=1");

    println!(
        "cargo:BINDGEN_OUTPUT_FILE={}",
        bindgen_outfilename.display()
    );
}

#[cfg(feature = "riot-rs")]
fn get_riot_var(riot_builddir: &str, var: &str) -> String {
    let output = std::process::Command::new("sh")
        .arg("-c")
        .arg(format!(
            "{} make --no-print-directory -C {} TOOLCHAIN=llvm info-debug-variable-{}",
            "WARNING_EXTERNAL_MODULE_DIRS=0", riot_builddir, var
        ))
        .output()
        .unwrap()
        .stdout;
    String::from_utf8_lossy(output.as_slice()).trim_end().into()
}
