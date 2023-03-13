// Copyright (C) 2020 Christian Amsüss
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]

use riot_wrappers::{riot_main, println};
use riot_wrappers::{gcoap, thread, ztimer, gnrc};

use coap_handler_implementations::{ReportingHandlerBuilder, HandlerBuilder};

extern crate rust_riotmodules;

riot_main!(main);

fn main(tokens: riot_wrappers::thread::StartToken) -> ! {
    extern "C" {
        fn do_vfs_init();
    }

    unsafe { do_vfs_init() };

    let mainlog = coap_message_demos::log::Log::start_once();

    let handler = coap_message_demos::full_application_tree(Some(mainlog))
        .below(&["ps"], riot_coap_handler_demos::ps::ps_tree())
        .below(&["vfs"], riot_coap_handler_demos::vfs::vfs("/const"))
        .with_wkc()
        ;
    let mut handler = riot_wrappers::coap_handler::GcoapHandler(handler);

    let mut listener = gcoap::SingleHandlerListener::new_catch_all(&mut handler);

    gcoap::scope(|greg| {
        greg.register(&mut listener);

        println!("CoAP server ready; waiting for interfaces to settle before reporting addresses...");
        // This goes into the /log resource
        log::info!("CoAP server is ready.");

        // Packing the clock for a single sleep doesn't save measurable runtime; this would be more
        // relevant if sleep were repeated.
        let sectimer = tokens.in_thread().promote(ztimer::Clock::sec());
        sectimer.sleep_ticks(2);

        for netif in gnrc::Netif::all() {
            println!("Active interface from PID {:?} ({:?})", netif.pid(), netif.pid().get_name().unwrap_or("unnamed"));
            match netif.ipv6_addrs() {
                Ok(addrs) => {
                    for a in &addrs {
                        println!("    Address {:?}", a);
                    }
                }
                _ => {
                    println!("    Does not support IPv6.");
                }
            }
        }

        // Sending main thread to sleep; can't return or the Gcoap handler would need to be
        // deregistered (which it can't).
        loop { thread::sleep(); }
    })
}
