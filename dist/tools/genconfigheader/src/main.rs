// SPDX-FileCopyrightText: 2026 Marian Buschsieweke
// SPDX-License-Identifier: LGPL-2.1-only

/// Command line tool to process CFLAGS from the environment and dump the
/// macro definitions into a header and the unprocessed flags into a makefile.
///
/// Usage:
/// ```sh
/// env CFLAGS="..." genconfigheader path/to/riotbuild.h path/to/cflags.mk
/// ```
///
/// It solves the following problems:
///
/// 1. Some prehistory systems have strict limits on how many paramters to pass
///    to an executable
/// 2. Help with correctness for incremental builds:
///     - A change in CFLAGS should cause a rebuild of all C files
///     - Build system can track modification timestamps of files easily, but
///       have no way of tracking changes in environment variables
///     - This writes a SHA256 hash of the CFLAGS into the generated header file
///       and regenerates it if and only if the hash written and computed differ
///     - letting all compilation target depend on the generated header then
///       solves the issue
/// 3. De-duplication and conflict checking
///     - The heavy use of CFLAGS for configuration parameters in RIOT easily
///       leads to issues where e.g. to modules independently from each other
///       e.g. inject a CFLAG to increase a default buffer size
///     - This tool can de-duplicate the flags and bail out early on conflicts
use std::borrow::Cow;
use std::collections::HashMap;
use std::env;
use std::fs::OpenOptions;
use std::io::{self, Read, Seek, SeekFrom, Write};
use std::process;

include!("header_prologue.rs");

/// Processes CFLAGS to generate header defines and makefile entries.
///
/// # Arguments
/// * `cflags` - CFLAGS string to process.
/// * `riotbuild_h` - Write handle for header file.
/// * `cflags_mk` - Write handle for makefile.
///
/// # Returns
/// `std::io::Result<()>` - Success or error.
fn process_cflags<W1: Write, W2: Write>(
    cflags: &str,
    riotbuild_h: &mut W1,
    cflags_mk: &mut W2,
) -> std::io::Result<()> {
    write!(cflags_mk, "CFLAGS :=")?;

    let mut macros: HashMap<&str, Option<String>> = HashMap::new();

    let words = genconfigheader::split_shell_parameters(cflags)?;

    for word in words {
        if let Some(rest) = word.strip_prefix("-D") {
            let (line, name, value) = match rest.split_once('=') {
                Some((name, value)) => {
                    let value = genconfigheader::unshell_param(value)?;
                    (format!("#define {} {}\n", name, value), name, value)
                }
                None => (format!("#define {} 1\n", rest), rest, Cow::Borrowed("1")),
            };

            match macros.get(name) {
                Some(prev) if prev.as_deref() == Some(value.as_ref()) => continue,
                Some(_) => {
                    return Err(std::io::Error::new(
                        io::ErrorKind::InvalidInput,
                        format!("Conflicting macro definitions for \"{}\"", name),
                    ));
                }
                None => {
                    macros.insert(name, Some(value.to_string()));
                }
            }

            riotbuild_h.write_all(line.as_bytes())?;
        } else if let Some(name) = word.strip_prefix("-U") {
            match macros.get(name) {
                Some(None) => continue,
                Some(_) => {
                    return Err(std::io::Error::new(
                        io::ErrorKind::InvalidInput,
                        format!("Conflicting macro (un-)definitions for \"{}\"", name),
                    ));
                }
                None => {
                    macros.insert(name, None);
                }
            }
            writeln!(riotbuild_h, "#undef {}", name)?;
        } else {
            write!(cflags_mk, " {}", word)?;
        }
    }

    writeln!(cflags_mk)?;

    Ok(())
}

#[cfg(test)]
mod tests_process_cflags {
    use super::*;

    #[test]
    fn process_cflags_input_is_only_defines() {
        let input = "-DFOO -DBAR=123";

        let mut header = Vec::new();
        let mut makefile = Vec::new();

        process_cflags(input, &mut header, &mut makefile).unwrap();

        let header = String::from_utf8(header).unwrap();
        let makefile = String::from_utf8(makefile).unwrap();

        assert!(header.contains("#define FOO 1\n"));
        assert!(header.contains("#define BAR 123\n"));

        assert_eq!(makefile, "CFLAGS :=\n");
    }

    #[test]
    fn prcoess_cflags_input_is_mixed_bag() {
        let input = r#"-DFOO -DBAR="func(6, 7)" -UBAZ -DMSG="\"Hi there!\"" -ffile-prefix-map="/home/foo/my repos/RIOT=" -Iinclude -ffunction-sections"#;

        let mut header = Vec::new();
        let mut makefile = Vec::new();

        process_cflags(input, &mut header, &mut makefile).unwrap();

        let header = String::from_utf8(header).unwrap();
        let makefile = String::from_utf8(makefile).unwrap();

        assert_eq!(
            header,
            r#"#define FOO 1
#define BAR func(6, 7)
#undef BAZ
#define MSG "Hi there!"
"#
        );
        assert_eq!(
            makefile,
            r#"CFLAGS := -ffile-prefix-map="/home/foo/my repos/RIOT=" -Iinclude -ffunction-sections
"#
        );
    }

    #[test]
    fn process_cflags_input_contains_no_defines() {
        let input = "-Iinclude -Os -ffunction-sections";

        let mut header = Vec::new();
        let mut makefile = Vec::new();

        process_cflags(input, &mut header, &mut makefile).unwrap();

        let header = String::from_utf8(header).unwrap();
        let makefile = String::from_utf8(makefile).unwrap();

        assert_eq!(header, "");
        assert_eq!(makefile, "CFLAGS := -Iinclude -Os -ffunction-sections\n");
    }

    #[test]
    fn process_cflags_input_contains_duplicates() {
        let input = "-DFOO=1 -DBAR -DFOO=1";

        let mut header = Vec::new();
        let mut makefile = Vec::new();

        process_cflags(input, &mut header, &mut makefile).unwrap();

        let header = String::from_utf8(header).unwrap();
        let makefile = String::from_utf8(makefile).unwrap();

        assert_eq!(
            header,
            r#"#define FOO 1
#define BAR 1
"#
        );
        assert_eq!(makefile, "CFLAGS :=\n");
    }

    #[test]
    fn process_cflags_input_contains_duplicates_without_value() {
        let input = "-DFOO -DBAR -DFOO -DBAZ -DFOO=1";

        let mut header = Vec::new();
        let mut makefile = Vec::new();

        process_cflags(input, &mut header, &mut makefile).unwrap();

        let header = String::from_utf8(header).unwrap();
        let makefile = String::from_utf8(makefile).unwrap();

        assert_eq!(
            header,
            r#"#define FOO 1
#define BAR 1
#define BAZ 1
"#
        );
        assert_eq!(makefile, "CFLAGS :=\n");
    }

    #[test]
    fn process_cflags_input_contains_conflicting_values() {
        let input = "-DFOO=1 -DBAR -DFOO=2";

        let mut header = Vec::new();
        let mut makefile = Vec::new();

        let result = process_cflags(input, &mut header, &mut makefile);
        assert!(result.is_err())
    }

    #[test]
    fn process_cflags_input_contains_conflicting_define_with_value_undefine() {
        let input = "-DFOO=1 -DBAR -UFOO";

        let mut header = Vec::new();
        let mut makefile = Vec::new();

        let result = process_cflags(input, &mut header, &mut makefile);
        assert!(result.is_err());
    }

    #[test]
    fn process_cflags_input_contains_conflicting_define_without_value_undefine() {
        let input = "-DFOO=1 -DBAR -UBAR";

        let mut header = Vec::new();
        let mut makefile = Vec::new();

        let result = process_cflags(input, &mut header, &mut makefile);
        assert!(result.is_err());
    }
}

/// Program entry point
///
/// # Returns
/// `io::Result<()>` - Success or error.
fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();

    if args.len() != 3 {
        eprintln!("usage: {} <header-file> <makefile>", args[0]);
        process::exit(1);
    }

    let riotbuild_h_path = &args[1];
    let cflags_mk_path = &args[2];

    let mut riotbuild_h = OpenOptions::new()
        .create(true)
        .truncate(false)
        .read(true)
        .write(true)
        .open(riotbuild_h_path)?;

    let (new_hash, cflags) = genconfigheader::get_cflags_and_hash()?;
    let mut old_hash = String::new();
    riotbuild_h.read_to_string(&mut old_hash)?;
    old_hash = old_hash
        .lines()
        .last()
        .and_then(|line| line.trim().strip_prefix("/* CFLAGS hash: "))
        .and_then(|s| s.strip_suffix(" */"))
        .unwrap_or("")
        .to_string();

    if old_hash == new_hash {
        println!(
            "genconfigheader: CFLAGS unchanged, not regenerating {} and {}",
            riotbuild_h_path, cflags_mk_path
        );
        drop(riotbuild_h);
        return Ok(());
    }

    let mut cflags_mk = OpenOptions::new()
        .create(true)
        .write(true)
        .truncate(true)
        .open(cflags_mk_path)?;

    riotbuild_h.set_len(0)?;
    riotbuild_h.seek(SeekFrom::Start(0))?;
    riotbuild_h.write_all(HEADER_PROLOGUE.as_bytes())?;
    process_cflags(&cflags, &mut riotbuild_h, &mut cflags_mk)?;
    cflags_mk.flush()?;
    drop(cflags_mk);
    writeln!(riotbuild_h, "/* CFLAGS hash: {} */", new_hash)?;
    riotbuild_h.flush()?;
    drop(riotbuild_h);

    if old_hash.is_empty() {
        println!(
            "genconfigheader: generated {} and {}",
            riotbuild_h_path, cflags_mk_path
        );
    } else {
        println!(
            "genconfigheader: regenerated {} and {} (old hash: {}, new hash: {})",
            riotbuild_h_path, cflags_mk_path, old_hash, new_hash
        );
    }

    Ok(())
}
