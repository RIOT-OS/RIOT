#![feature(lang_items)]
#![no_main]
#![no_std]

mod lang_items {
    #[lang = "panic_fmt"]
    extern fn panic_fmt() {}

    #[lang = "eh_personality"]
    fn eh_personality() {}
}

extern crate fmt;

use fmt::Stdout;
use core::fmt::Write;

#[no_mangle]
pub fn main() {
	let mut out = Stdout {};
	out.write_str("Hello World!\n");
}
