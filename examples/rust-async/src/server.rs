//! This is an adaptation of examples/rust-gcoap server code for easy
//! async initialization.

use coap_handler::Handler;
use coap_handler_implementations::{ReportingHandlerBuilder, HandlerBuilder};
use riot_wrappers::coap_handler::v0_2::GcoapHandler;
use riot_wrappers::gcoap::SingleHandlerListener;

fn new_gcoap_handler() -> GcoapHandler<impl Handler> {
    let handler = coap_message_demos::full_application_tree(None)
        .below(&["ps"], riot_coap_handler_demos::ps::ps_tree())
        .below(&["vfs"], riot_coap_handler_demos::vfs::vfs("/const"))
        .below(&["saul"], riot_coap_handler_demos::saul::SaulHandler::new(&["saul"]))
        .with_wkc();

    GcoapHandler(handler)
}

fn new_gcoap_listener(handler: &'static mut GcoapHandler<impl Handler>)
    -> SingleHandlerListener<'static, GcoapHandler<impl Handler>> {
    SingleHandlerListener::new_catch_all(handler)
}

fn init_vfs() {
    extern "C" {
        fn do_vfs_init();
    }

    unsafe { do_vfs_init() };
}

pub async fn start() {
    use riot_wrappers::println;
    use crate::util::{get_static, sleep_forever};

    init_vfs();

    let mut handler = new_gcoap_handler();
    let mut listener = new_gcoap_listener(get_static(&mut handler));
    riot_wrappers::gcoap::register(get_static(&mut listener));
    println!("CoAP server ready");

    // this prevents `handler` and `listener` from dropping
    sleep_forever().await;

    panic!("should be never reached");
}