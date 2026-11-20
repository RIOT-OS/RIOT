extern crate bindgen;
extern crate shlex;

use bindgen::builder;
use std::env;
use std::fmt::Write;
use std::path::PathBuf;

use serde_json::json;

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

    println!("cargo:rerun-if-changed=riot-bindgen.h");

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

    let bindings = builder()
        .header("riot-bindgen.h")
        .clang_args(&cflags)
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

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    let bindgen_outfilename = out_path.join("bindings.rs");
    bindings
        .write_to_file(&bindgen_outfilename)
        .expect("Couldn't write bindings!");
    // Store for inspection for markers; see there
    let mut bindgen_output = Vec::<u8>::new();
    bindings
        .write(Box::new(&mut bindgen_output))
        .expect("String writing never fails");
    let bindgen_output = std::str::from_utf8(&bindgen_output).expect("Rust source code is UTF-8");

    // Build a compile_commands.json, and run C2Rust
    //
    // The output is cleared beforehand (for c2rust no-ops when an output file is present), and the
    // input is copied to OUT_DIR as that's the easiest way to get c2rust to put the output file in
    // a different place -- and because some additions are generated anyway.

    let c2rust_infile = "riot-c2rust.h";
    // Follows from c2rust_infile and C2Rust's file name translation scheme
    let c2rust_output = out_path.join("riot_c2rust.rs");
    let headercopy = out_path.join(c2rust_infile);
    println!("cargo:rerun-if-changed=riot-c2rust.h");

    std::fs::copy("riot-headers.h", out_path.join("riot-headers.h"))
        .expect("Failed to copy over header file");

    // These constant initializers are unusable without knowledge of which type they're for; adding
    // the information here to build explicit consts
    let macro_functions = [
        ("SOCK_IPV4_EP_ANY", "sock_udp_ep_t", None, true, None),
        ("SOCK_IPV6_EP_ANY", "sock_udp_ep_t", None, true, None),
        ("MUTEX_INIT", "mutex_t", None, true, None),
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
        ("TIMER_DEV", "timer_t", Some("unsigned num"), false, None),
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

    let mut c_code = String::new();
    std::fs::File::open("riot-c2rust.h")
        .expect("Failed to open riot-c2rust.h")
        .read_to_string(&mut c_code)
        .expect("Failed to read riot-c2rust.h");

    for (macro_name, return_type, mut args, _is_const, fallback_value) in macro_functions.iter() {
        let expression = match args {
            None => macro_name.to_string(),
            Some("void") => format!("{macro_name}()"),
            // This could be an extra field of another enum variant too -- its point is to
            // introduce special handling for macros that are not just used with no arguments, but
            // are really more of an ifdef guard.
            //
            // Those would also *work* with `None`, but it creates code such as `#define
            // LED0_IS_PRESENT` / `int result =
            // LED0_IS_PRESENT;`, which is kind of accepted by C2Rust to mean 1 (which is the case
            // when passed through `-D` but maybe not by the one-argument `#define`), but also
            // shows an error in the C2Rust output, which can be misleading when there is a
            // different C2Rust error but those errors are also visible (as was the case in
            // <https://github.com/RIOT-OS/RIOT/issues/21079>).
            Some("defined") => {
                args = None;
                // No need to check further: the function is already inside an ifdef
                "1".to_string()
            }
            Some(args) => format!(
                "{macro_name}({})",
                args.split(", ")
                    .map(|s| &s[s.find(" ").expect("Non-void args need to have names")..])
                    // Not really essential concepturally, but .join is only available on
                    // slices, not on Iterator
                    .collect::<Vec<&str>>()
                    .join(", ")
            ),
        };

        // The ifdef guards make errors easier to spot: A "cannot find function
        // `macro_SOCK_IPV6_EP_ANY` in crate `riot_sys`" can lead one to check whether
        // SOCK_IPV6_EP_ANY is really defined, whereas if the macro is missing, C2Rust would
        // produce a run-time panic, and the compiler would reject that in a const function.
        //
        // This is more reliable than the previous approach of trying to defined a `-DSOME_MODULE`
        // condition, also because there may not even be a module that gives a precise condition.
        if *return_type == "void" {
            // in C, assigning and returning void is special
            write!(
                c_code,
                r"

#ifdef {macro_name}
{return_type} macro_{macro_name}({args}) {{
    {expression};
}}
                ",
                return_type = return_type,
                expression = expression,
                args = args.unwrap_or("void"),
            )
        } else {
            write!(
                c_code,
                r"

#ifdef {macro_name}
{return_type} macro_{macro_name}({args}) {{
    {return_type} result = {expression};
    return result;
}}
                ",
                return_type = return_type,
                expression = expression,
                args = args.unwrap_or("void"),
            )
        }
        .unwrap();

        if let Some(fallback_value) = fallback_value {
            writeln!(
                c_code,
                r"
#else
{return_type} macro_{macro_name}({args}) {{
    return {fallback_value};
}}
                     ",
                args = args.unwrap_or("void"),
            )
            .unwrap();
        }
        writeln!(c_code, r" #endif").unwrap();
    }

    let mut outfile =
        std::fs::File::create(&headercopy).expect("Failed to open temporary riot-c2rust.h");
    outfile
        .write_all(c_code.as_bytes())
        .expect("Failed to write to riot-c2rust.h");
    outfile
        .sync_all()
        .expect("failed to write to riot-c2rust.h");

    if cc.find("clang") == None {
        panic!("riot-sys only accepts clang style CFLAGS. RIOT can produce them using the compile_commands tool even when using a non-clang compiler, such as GCC.");
    };

    let arguments: Vec<_> = core::iter::once("any-cc".to_string())
        .chain(cflags.into_iter())
        .chain(core::iter::once(c2rust_infile.to_string()))
        .collect();
    let compile_commands = json!([{
        "arguments": arguments,
        "directory": out_path,
        "file": c2rust_infile,
    }]);
    let compile_commands_name = out_path.join("compile_commands.json");

    let mut compile_commands_file = std::fs::File::create(compile_commands_name.clone())
        .expect("Failed to create compile_commands.json");
    serde_json::to_writer_pretty(&mut compile_commands_file, &compile_commands)
        .expect("Failed to write to compile_commands.json");
    compile_commands_file
        .sync_all()
        .expect("Failed to write to compile_commands.json");

    let compile_commands_name = compile_commands_name
        .to_str()
        .expect("Inexpressible path name");

    println!("cargo:rerun-if-env-changed=C2RUST");
    println!("cargo:rerun-if-env-changed=PATH");
    let c2rust = std::env::var("C2RUST").unwrap_or_else(|_| "c2rust".to_string());
    let c2rust_version = std::process::Command::new(&c2rust)
        .args(&["--version"])
        .output()
        .expect("C2Rust version check did not complete")
        .stdout;
    let c2rust_version = String::from_utf8_lossy(&c2rust_version);
    print!("C2Rust binary {}, version: {}", c2rust, c2rust_version);
    // Version won't tell us whether --translate-const-macros needs a TRANSLATE_CONST_MACROS
    // argument or not, because people can also install from git
    let c2rust_help = std::process::Command::new(&c2rust)
        .args(&["transpile", "--help"])
        .output()
        .expect("C2Rust --help did not work")
        .stdout;
    let c2rust_help =
        core::str::from_utf8(&c2rust_help).expect("C2Rust help had non-UTF8 characters");
    let translate_const_macros_arg = if c2rust_help.contains("TRANSLATE_CONST_MACROS") {
        "--translate-const-macros=conservative"
    } else {
        // versions up to 0.20
        "--translate-const-macros"
    };
    // FIXME: This does not rat on the used files. Most are probably included from riot-bindgen.h
    // anyway, tough.
    println!("Running C2Rust on {}", compile_commands_name);
    let status = std::process::Command::new(&c2rust)
        .args(&[
            "transpile",
            compile_commands_name,
            "--emit-modules",
            "--emit-no-std",
            translate_const_macros_arg,
            "--overwrite-existing",
            "--fail-on-error",
        ])
        .status()
        .expect("C2Rust failed");
    if !status.success() {
        println!(
            "cargo:warning=C2Rust failed with error code {}, exiting",
            status
        );
        std::process::exit(status.code().unwrap_or(1));
    }

    // Some fix-ups to the C2Rust output
    // (could just as well call sed...)

    use std::io::{Read, Write};

    let mut rustcode = String::new();
    std::fs::File::open(c2rust_output)
        .expect("Failed to open riot_c2rust.rs")
        .read_to_string(&mut rustcode)
        .expect("Failed to read from riot_c2rust.rs");

    rustcode = rustcode.replace("use ::libc;\n", "");

    if !c2rust_version.contains("+git-for-riot") && c2rust_version.contains("C2Rust 0.15") {
        // Old C2Rust still generate old-style ASM -- workaround for https://github.com/immunant/c2rust/issues/306
        rustcode = rustcode.replace(" asm!(", " llvm_asm!(");
    }

    // Workaround for https://github.com/immunant/c2rust/issues/372
    rustcode = rustcode.replace("::core::intrinsics::", "crate::intrinsics_replacements::");

    println!("cargo:rerun-if-env-changed=CARGO_FEATURE_KEEP_EXTERN_TYPES");
    if env::var("CARGO_FEATURE_KEEP_EXTERN_TYPES").is_err() {
        // For documentation on why we do this, see include in src/inline.rs.

        let pubtypepattern = regex::Regex::new("pub type (?P<type>[a-zA-Z0-9_]+);").unwrap();
        let pubtypes = pubtypepattern
            .captures_iter(&rustcode)
            .map(|m| m.name("type").unwrap().as_str());

        let pubtype_replacements = out_path.join("pubtype_replacements.rs");
        let mut pubtype_replacements_file = std::fs::File::create(pubtype_replacements)
            .expect("Failed to create pubtype_replacements.rs");

        for pt in pubtypes {
            writeln!(
                pubtype_replacements_file,
                "pub type {} = [u8; isize::MAX as _];",
                pt
            )
            .expect("Failed to write to pubtype_replacements.rs");
        }

        rustcode = pubtypepattern
            .replace_all(&rustcode, "/* $0 */")
            .to_string();
    }

    // On 64-bit native, what gets emitted as vprintf(_, _, _: __builtin_va_list) gets emitted as
    // vprintf(_, _, _: core::ffi::VaList), which is unsupported in stable -- but we don't use that
    // function, it's just an unfortunate side effect of --preserve-unused-functions. This quick
    // workaround enables building and ensures that the function is never called.
    rustcode = rustcode.replace("::core::ffi::VaList", "::core::convert::Infallible");
    rustcode = rustcode.replace("__arg.as_va_list()", "__arg");

    // Replace the function declarations with ... usually something pub, but special considerations
    // may apply
    let mut rustcode_functionsreplaced = String::new();
    let function_original_prefix = r#"unsafe extern "C" fn "#;
    let mut functionchunks = rustcode.split(function_original_prefix);
    rustcode_functionsreplaced.push_str(
        functionchunks
            .next()
            .expect("Split produces at least a hit"),
    );

    for chunk in functionchunks {
        let funcname = &chunk[..chunk.find('(').expect("Function has parentheses somewhere")];
        let macro_details = if funcname.len() > 5 && &funcname[..6] == "macro_" {
            macro_functions
                .iter()
                .filter(|(macro_name, _, _, _, _)| &funcname[6..] == *macro_name)
                .next()
        } else {
            None
        };
        let new_prefix = match (funcname, macro_details) {
            // used as a callback, therefore does need the extern "C" -- FIXME probably worth a RIOT issue
            ("_evtimer_msg_handler" | "_evtimer_mbox_handler", _) => function_original_prefix,

            // Assigned by CMSIS to the const that is being overridden and thus needs its original
            // "C" type; see also riot-c2rust.h. (Actually using it would cause a linker error
            // anyway).
            ("__masked_builtin_arm_get_fpscr" | "__masked_builtin_arm_set_fpscr", _) => {
                function_original_prefix
            }

            // same problem but from C2Rust's --translate-const-macros
            ("__NVIC_SetPriority", _) => function_original_prefix,

            // As below (no need for extern), and they are const as declared ni the macro_functions
            // list.
            (_, Some((_, _, _, is_const, _))) => {
                // No "pub" because that's already a "pub" in front of it, they were never static
                match is_const {
                    // FIXME: These should be unsafe -- just because most of them are const doesn't
                    // necessrily mean they're safe (just the first few happened to be, but that's
                    // not this crate's place to assert)
                    true => "const unsafe fn ",
                    false => "unsafe fn ",
                }
            }

            // C2Rust transpiles these into Rust with conflicting lifetimes, see
            // https://github.com/immunant/c2rust/issues/309
            //
            // Simply disabling them here because they aren't used by any other inline code (and
            // will, when the manual llvm_asm to asm changes are added to riot-sys, not have manual
            // asm conversions on top of that).
            ("__SMLALD" | "__SMLALDX" | "__SMLSLD" | "__SMLSLDX", _) => {
                "#[cfg(c2rust_fixed_309)]\npub unsafe fn "
            }

            // The rest we don't need to call through the extern convention, but let's please make
            // them pub to be usable
            _ => "pub unsafe fn ",
        };
        rustcode_functionsreplaced.push_str(new_prefix);
        rustcode_functionsreplaced.push_str(chunk);
    }

    rustcode = rustcode_functionsreplaced;

    // We don't even need that function any more, we just have that in there so that c2rust gives
    // us its callees as functions. (In bindgen, we'd just list the functions we'd like to have).
    rustcode = rustcode.replace(
        "pub pub unsafe fn use_everything()",
        "#[cfg(false)] unsafe fn use_everything()",
    );
    // The macro_ wrappers are already `pub` because they are regular functions we create here that
    // just go around the macro code; removing the double pub.
    rustcode = rustcode.replace("pub pub unsafe fn ", "pub unsafe fn ");

    let output_replaced = out_path.join("riot_c2rust_replaced.rs");
    std::fs::File::create(output_replaced)
        .expect("Failed to create riot_c2rust_replaced.rs")
        .write(rustcode.as_bytes())
        .expect("Failed to write to riot_c2rust_replaced.rs");

    // Pub uses of inline right into the main lib.rs
    //
    // This is primarily for things that can really come from either backend (eg. irq functions
    // that are regular on native but static inline on others), and for convenience stuff like
    // macro_.
    //
    // Some functions are also in because they're innocuous enough.
    //
    // If (eg. on some platform but not on others) any function here is not an inline function,
    // that does not hurt; the entry doesn't do anything on these then. (But it is especially
    // valuable, as it ensures that on the *other* platforms it's still available with the same
    // Rust name).
    let mut toplevel_from_inline: Vec<String> = [
        "bluetil_ad_add_flags",
        "coap_get_code_raw",
        "coap_get_total_hdr_len",
        "gnrc_netapi_dispatch_send",
        "gnrc_netif_ipv6_addrs_get",
        "gnrc_netreg_entry_init_pid",
        "gpio_is_valid",
        "irq_disable",
        "irq_is_enabled",
        "irq_is_in",
        "irq_restore",
        "mutex_trylock",
        "mutex_lock",
        "pid_is_valid",
        "shell_run_forever",
        "sock_udp_recv",
        "sock_udp_send",
        "thread_get",
        "thread_getpid",
        "thread_get_unchecked",
        "ztimer_spin",
        "ws281x_prepare_transmission",
        "ws281x_end_transmission",
        // because when defined through RIOT's af.h these are enums and thus unix_af_t prefixed.
        "AF_UNSPEC",
        "AF_UNIX",
        "AF_PACKET",
        "AF_INET",
        "AF_INET6",
    ]
    .iter()
    .map(|name| name.to_string())
    .collect();
    for (macro_name, _, _, _, _) in macro_functions.iter() {
        toplevel_from_inline.push(format!("macro_{}", macro_name));
    }
    let toplevel_from_inline: Vec<String> = toplevel_from_inline
        .drain(..)
        .filter(|s: &String| {
            // Not just matching on `pub fn`: `irq_disable` on native is visible as `extern "C" {
            // fn irq_disable(); }`, and that should not trigger going through C2Rust.
            rustcode.contains(&format!("pub fn {}(", s))
                || rustcode.contains(&format!("unsafe fn {}(", s))
                || rustcode.contains(&format!(" {}: ", s))
        })
        .collect();
    let toplevel_from_inline_filename = out_path.join("toplevel_from_inline.rs");
    std::fs::File::create(toplevel_from_inline_filename)
        .expect("Failed to create toplevel_from_inline.rs")
        .write(
            format!(
                "
               pub use inline::{{ {} }};
           ",
                toplevel_from_inline.join(",\n")
            )
            .as_bytes(),
        )
        .expect("Failed to write to toplevel_from_inline.rs");

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
