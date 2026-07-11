// SPDX-FileCopyrightText: 2026 Marian Buschsieweke
// SPDX-License-Identifier: LGPL-2.1-only

use sha2::{Digest, Sha256};
use std::borrow::Cow;
use std::io;

/// Encodes bytes as a hexadecimal string.
///
/// # Arguments
/// * `bytes` - Byte slice to encode.
///
/// # Returns
/// `String` - Hexadecimal representation.
pub fn hex_encode(bytes: &[u8]) -> String {
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
pub fn get_cflags_and_hash() -> io::Result<(String, String)> {
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
pub fn split_shell_parameters(input: &str) -> Result<Vec<&str>, io::Error> {
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
pub fn unshell_param(s: &str) -> Result<Cow<'_, str>, io::Error> {
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
