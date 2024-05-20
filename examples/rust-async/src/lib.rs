#![no_std]
#![feature(type_alias_impl_trait)]

use riot_wrappers::{riot_main, println};


extern crate rust_riotmodules;

mod gcoap;
mod util;
mod executor;
mod runtime;

// WIP
mod stream;
//mod shell;

riot_main!(main);

fn main() {
    if 0 == 1 { gcoap::sync_main(); panic!("debug gcoap ok"); }

    runtime::Runtime::new().run();
}

async fn get_number() -> u32 { 42 }

async fn async_main() {
    println!("async_main(): ^^");

    for _ in 0..3 {
        let number = get_number().await;
        println!("number: {}", number);
    }

    //stream::foo(); // !!!!
    panic!("ok");
}