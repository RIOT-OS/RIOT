use std::fs;
use std::process::Command;

// Call `sed` to strip any line that is not a preprocessor directive from the
// intput and emit the result into a source file containing it as constant
fn main() {
    // Run sed to strip comments
    let output = Command::new("sed")
        .args(&["-n", "-e", "/^#.*/ p", "riotbuild-prefix.h"])
        .output()
        .expect("failed to execute sed");

    let boilderplate_only_macros = String::from_utf8_lossy(&output.stdout);

    let headder_prologue_rs = format!(
        "pub const HEADER_PROLOGUE: &str = r###\"/* auto generated, do not edit by hand */\n{}\"###;",
        boilderplate_only_macros
    );
    fs::write("src/header_prologue.rs", headder_prologue_rs).expect("failed to write header");
}
