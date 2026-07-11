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
use sha2::{Digest, Sha256};
use std::borrow::Cow;
use std::collections::HashMap;
use std::env;
use std::fs::OpenOptions;
use std::io::{self, Read, Seek, SeekFrom, Write};
use std::process;

include!("header_prologue.rs");

/// Encodes bytes as a hexadecimal string.
///
/// # Arguments
/// * `bytes` - Byte slice to encode.
///
/// # Returns
/// `String` - Hexadecimal representation.
fn hex_encode(bytes: &[u8]) -> String {
    let mut s = String::with_capacity(bytes.len() * 2);
    for b in bytes {
        use std::fmt::Write;
        write!(&mut s, "{:02x}", b).unwrap();
    }
    s
}

/// Gets CFLAGS from environment and computes their SHA256 hash.
///
/// # Returns
/// `io::Result<(String, String)>` - (hash, cflags string).
fn get_cflags_and_hash() -> io::Result<(String, String)> {
    let buffer =
        std::env::var("CFLAGS").map_err(|e| io::Error::new(io::ErrorKind::InvalidInput, e))?;

    let digest = Sha256::digest(buffer.as_bytes());
    let hash = hex_encode(&digest);

    Ok((hash, buffer))
}

#[cfg(test)]
mod tests_read_cflags_and_hash {
    use super::*;
    use temp_env::with_vars;

    #[test]
    fn get_cflags_and_hash_returns_expected_hash() {
        let input = "-DDEBUG -Iinclude -O2";
        let (hash, buffer) = with_vars(vec![("CFLAGS", Some(input))], || {
            get_cflags_and_hash().unwrap()
        });

        assert_eq!(buffer, input);
        // Use `printf '-DDEBUG -Iinclude -O2' | sha256sum` to get the input
        assert_eq!(
            hash,
            "745b8d6341b451a10e9611d5ec83c87a4d0f623e458cfd87c76115b1f0f2ee88"
        );
    }

    #[test]
    fn get_cflags_and_hash_fails_on_missing_cflags() {
        let result = with_vars(vec![("CFLAGS", None::<&str>)], || get_cflags_and_hash());

        assert!(result.is_err());
    }
}

/// Splits a string into shell parameters as a shell would do as unmodified
/// slices of the original string
///
/// # Arguments
/// * `input` - Input string to split.
///
/// # Returns
/// `Vec<String>` - Vector of slices
fn split_shell_parameters(input: &str) -> Result<Vec<&str>, io::Error> {
    let mut words: Vec<&str> = Vec::new();

    let mut chars = input.char_indices().peekable();

    let mut in_single = false;
    let mut in_double = false;
    let mut start: Option<usize> = None;

    while let Some((i, c)) = chars.next() {
        if start.is_none() && !c.is_whitespace() {
            start = Some(i)
        }

        match c {
            '\'' if !in_double => {
                in_single = !in_single;
            }
            '"' if !in_single => {
                in_double = !in_double;
            }
            '\\' => {
                chars.next();
            }
            c if c.is_whitespace() && !in_single && !in_double => {
                if let Some(s) = start.take() {
                    words.push(&input[s..i]);
                }
            }
            _ => continue,
        }
    }

    if let Some(s) = start {
        words.push(&input[s..]);
    }

    if in_double || in_single {
        return Err(io::Error::other("Unbalenced quotation"));
    }

    Ok(words)
}

#[cfg(test)]
mod tests_split_shell_parameters {
    use super::*;

    #[test]
    fn split_shell_parameters_mixed_test() {
        let input = r#"-DFOO -DBAR='func(6, 7)' -DBAZ="func2(42, 1337)" -DMSG="\"Hi there!\"""#;
        let words = split_shell_parameters(input).unwrap();

        assert_eq!(
            words,
            vec![
                "-DFOO",
                "-DBAR='func(6, 7)'",
                r#"-DBAZ="func2(42, 1337)""#,
                r#"-DMSG="\"Hi there!\"""#
            ]
        );
    }

    #[test]
    fn split_shell_parameters_unbalanced_quotation() {
        let input = "-DFOO -DBAR='func(6, 7) -BAZ";
        let result = split_shell_parameters(input);

        assert!(result.is_err());
    }
}

/// Removes surrounding quotes from a string and, for valid input, processes
/// escaping just like a shell would do for each parameter.
///
/// Note: Do not expect this function to fail on invalid escaping a proper
///       shell would do.
///
/// # Arguments
/// * `s` - String to process.
///
/// # Returns
/// `Result<String, io::Error>` - "un-shell-ed" parameter or error.
fn unshell_param(s: &str) -> Result<Cow<'_, str>, io::Error> {
    let chars = s.chars().peekable();

    // we assume mostly strings do not contain any quation or backslash chars
    // and iterate twice if they do
    for c in chars {
        match c {
            '\\' | '"' | '\'' => {
                return unshell_param_slow(s).map(Cow::Owned);
            }
            _ => {}
        }
    }
    Ok(Cow::Borrowed(s))
}

/// Workhorse for `unshell_param()` that does the actual transformation. The
/// idea is that for most `-DPARAM=value` the `value` part contains no escaping
/// nor quotes, so `unquote()` optimizes heavily for that case. This function
/// is for the exceptions where modifications are needed and is optmized for
/// readability rather than speed.
///
/// # Arguments
/// * `b` - Byte slice to parse.
///
/// # Returns
/// `Result<String, io::Error>` - "un-shell-ed" parameter or error.
///
fn unshell_param_slow(s: &str) -> Result<String, io::Error> {
    let mut out = String::new();

    let mut chars = s.chars().peekable();

    while let Some(c) = chars.next() {
        match c {
            '\\' => match chars.next() {
                Some(c) => out.push(c),
                _ => {
                    return Err(io::Error::other("Missing char after `\\`"));
                }
            },

            '"' => {
                unshell_param_till_end_of_quote(&mut out, &mut chars, '"')?;
            }

            '\'' => {
                unshell_param_till_end_of_quote(&mut out, &mut chars, '\'')?;
            }

            c => {
                out.push(c);
            }
        }
    }

    Ok(out)
}

/// Unescapes the "body" of a quote and consumes all chars until the "closing"
/// quotation sign is found.
///
/// # Arguments
/// * `out` - Output string to append to.
/// * `chars` - Iterator over the input buffer
/// * `quot` - Quote character (single or double).
///
/// # Returns
/// `Result<usize, io::Error>` - New index after closing quote, or error.
fn unshell_param_till_end_of_quote(
    out: &mut String,
    chars: &mut impl Iterator<Item = char>,
    quot: char,
) -> Result<(), io::Error> {
    while let Some(c) = chars.next() {
        match c {
            '\\' => match chars.next() {
                Some(c) => out.push(c),
                _ => {
                    return Err(io::Error::other("Missing char after `\\`"));
                }
            },

            c if c == quot => {
                return Ok(());
            }

            c => out.push(c),
        }
    }

    Err(io::Error::other(format!(
        "Missing closing quote '{}'",
        quot
    )))
}

#[cfg(test)]
mod tests_unshell_param {
    use super::*;

    #[test]
    fn unshell_param_no_quotes() {
        assert_eq!(unshell_param("Foobar").unwrap(), "Foobar");
    }

    #[test]
    fn unshell_param_double_quotes() {
        assert_eq!(unshell_param(r#""foo bar""#).unwrap(), "foo bar");
    }

    #[test]
    fn unshell_param_single_quotes() {
        assert_eq!(unshell_param("'hello'").unwrap(), "hello");
    }

    #[test]
    fn unshell_param_mixed_quotes() {
        assert_eq!(unshell_param(r#""'hello'""#).unwrap(), "'hello'");
    }

    #[test]
    fn unshell_param_escaped_quotes_inside() {
        assert_eq!(
            unshell_param(r#""foo \"bar\" baz""#).unwrap(),
            r#"foo "bar" baz"#
        );
    }

    #[test]
    fn unshell_param_escaped_single_quotes() {
        assert_eq!(unshell_param(r#"'it\'s fine'"#).unwrap(), "it's fine");
    }

    #[test]
    fn unshell_param_escape_at_end_errors() {
        assert!(unshell_param(r#""unterminated\"#).is_err());
    }

    #[test]
    fn unshell_param_missing_closing_quote_double() {
        assert!(unshell_param(r#""foo bar"#).is_err());
    }

    #[test]
    fn unshell_param_missing_closing_quote_single() {
        assert!(unshell_param("'foo bar").is_err());
    }

    #[test]
    fn unshell_param_plain_text_with_backslash_escape() {
        assert_eq!(unshell_param(r#"hello\ world"#).unwrap(), "hello world");
    }
}

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

    let words = split_shell_parameters(cflags)?;

    for word in words {
        if let Some(rest) = word.strip_prefix("-D") {
            let (line, name, value) = match rest.split_once('=') {
                Some((name, value)) => {
                    let value = unshell_param(value)?;
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

    let (new_hash, cflags) = get_cflags_and_hash()?;
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
