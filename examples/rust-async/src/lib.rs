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
use util::get_static;

riot_main!(main);

fn main() {
    server::init_vfs();

    // We can assume that both `handler` and `listener` are never dropped since
    // `Runtime::run()` is a diverging function.
    let mut handler = server::new_gcoap_handler();
    let mut listener = server::new_gcoap_listener(get_static(&mut handler));

    riot_wrappers::gcoap::register(get_static(&mut listener));
    println!("CoAP server ready");

    runtime::Runtime::new().run();
}

async fn async_main() {
    use riot_wrappers::ztimer::{Clock, Ticks};

    Clock::msec().sleep_async(Ticks(2_000)).await;
    server::announce()
}