use std::process::Command;
use std::fs;

// Call `sed` to strip any line that is not a preprocessor directive from the
// intput and emit the result into a source file containing it as constant
fn main() {
    // Run sed to strip comments
    let output = Command::new("sed")
        .args(&[
            "-n",
            "-e", "/^#.*/ p",
            "riotbuild-prefix.h",
        ])
        .output()
        .expect("failed to execute sed");

    let processed = String::from_utf8_lossy(&output.stdout);

    // Write to a Rust file
    fs::write("src/header_prologue.rs", format!("pub const HEADER_PROLOGUE: &str = r###\"/* auto generated, do not edit by hand */\n{}\"###;", processed))
        .expect("failed to write header");
}
