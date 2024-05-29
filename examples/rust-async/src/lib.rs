#![no_std]
#![feature(type_alias_impl_trait)]

extern crate rust_riotmodules;

mod util;
mod executor;
mod runtime;
mod stream;
mod shell;
mod server;

use riot_wrappers::{riot_main, println};

riot_main!(main);

fn main() {
    runtime::Runtime::new().run();
}

async fn async_main() {
    println!("async Rust example application");

    util::sleep_msec(2000).await;
    util::announce_netif();
}