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
use util::{sleep_msec, get_static};

riot_main!(main);

fn main() {
    runtime::Runtime::new().run();
}

async fn async_main() {
    server::init_vfs();
    let mut handler = server::new_gcoap_handler();
    let mut listener = server::new_gcoap_listener(get_static(&mut handler));
    riot_wrappers::gcoap::register(get_static(&mut listener));
    println!("CoAP server ready");

    sleep_msec(2000).await;
    server::announce();

    // this prevents `handler` and `listener` from dropping
    loop { sleep_msec(2000).await; }

    // should be never reached
}