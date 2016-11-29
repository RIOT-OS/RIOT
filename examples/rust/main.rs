#![no_main]
#![no_std]

extern crate fmt;
extern crate common;

use fmt::Stdout;
use core::fmt::Write;

#[no_mangle]
pub fn main() {
	let mut out = Stdout {};
	out.write_str("Hello World!\n");
}
