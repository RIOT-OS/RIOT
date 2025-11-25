#![no_std]

use riot_wrappers::println;
use core::fmt::Write;

riot_wrappers::static_command!(static_hello_world, "hello_world", "Print a greeting", hello_world);

pub fn hello_world<'a>(_w: &mut impl Write, args: impl IntoIterator<Item=&'a str>) {
    let mut args = args.into_iter();
    let commandname = args.next().expect("How was this started without an argv[0]?");

    match args.next() {
        Some("--help") => println!("Usage: {commandname}"),
        None => println!("Hello RIOT!"),
        _ => println!("Invalid argument."),
    };
}
