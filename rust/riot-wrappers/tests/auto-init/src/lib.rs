#![no_std]

use riot_wrappers::println;
use riot_wrappers::riot_main;

riot_main!(main);

fn main() {
    println!("Main running");
}

riot_wrappers::auto_init!(auto_early, 1);
riot_wrappers::auto_init!(auto_late, 65535);

fn auto_early() {
    println!("Early auto initialization");
}

fn auto_late() {
    println!("Late auto initialization");
}
