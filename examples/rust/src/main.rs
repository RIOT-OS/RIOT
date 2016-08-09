#![feature(lang_items)]
#![no_main]
#![no_std]

mod lang_items {
    #[lang = "panic_fmt"]
    extern fn panic_fmt() {}

    #[lang = "eh_personality"]
    fn eh_personality() {}
}

// user code starts here

extern crate riot;

#[no_mangle]
pub fn main() {
    riot::fmt::print("Hello from RUST!\n");
}
