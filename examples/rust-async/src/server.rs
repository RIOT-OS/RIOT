use coap_handler::Handler;
use coap_handler_implementations::{ReportingHandlerBuilder, HandlerBuilder};
use riot_wrappers::coap_handler::v0_2::GcoapHandler;
use riot_wrappers::gcoap::SingleHandlerListener;

pub fn new_gcoap_handler() -> GcoapHandler<impl Handler> {
    let handler = coap_message_demos::full_application_tree(None)
        .below(&["ps"], riot_coap_handler_demos::ps::ps_tree())
        .below(&["vfs"], riot_coap_handler_demos::vfs::vfs("/const"))
        .below(&["saul"], riot_coap_handler_demos::saul::SaulHandler::new(&["saul"]))
        .with_wkc();

    GcoapHandler(handler)
}

pub fn new_gcoap_listener(handler: &'static mut GcoapHandler<impl Handler>)
    -> SingleHandlerListener<'static, GcoapHandler<impl Handler>> {
    SingleHandlerListener::new_catch_all(handler)
}

pub fn init_vfs() {
    extern "C" {
        fn do_vfs_init();
    }

    unsafe { do_vfs_init() };
}

pub fn announce() {
    use riot_wrappers::{gnrc, println};

    for netif in gnrc::Netif::all() {
        println!("active interface from PID {:?} ({:?})",
                 netif.pid(), netif.pid().get_name().unwrap_or("unnamed"));
        if let Ok(addrs) = netif.ipv6_addrs() {
            addrs.iter().for_each(|a| println!("    Address {:?}", a));
        } else {
            println!("    Does not support IPv6.");
        }
    }
}