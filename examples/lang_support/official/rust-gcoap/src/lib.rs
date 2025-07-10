// SPDX-FileCopyrightText: 2020 Christian Amsüss
// SPDX-License-Identifier: LGPL-2.1-only
#![no_std]

use riot_wrappers::{gcoap, gnrc, ztimer};
use riot_wrappers::{println, riot_main};

use coap_handler_implementations::{HandlerBuilder, ReportingHandlerBuilder};

extern crate rust_riotmodules;

riot_main!(main);

fn main() {
    extern "C" {
        fn do_vfs_init();
    }

    unsafe { do_vfs_init() };

    static PINGS: riot_coap_handler_demos::ping::PingPool =
        riot_coap_handler_demos::ping::PingPool::new();
    static PING_PASSIVE: riot_coap_handler_demos::ping_passive::PingHistoryMutex<
        { riot_coap_handler_demos::ping_passive::DEFAULT_SIZE },
    > = riot_coap_handler_demos::ping_passive::PingHistoryMutex::new();

    let handler = coap_message_demos::full_application_tree(None)
        .below(&["ps"], riot_coap_handler_demos::ps::ps_tree())
        .below(&["led"], riot_coap_handler_demos::led::all_leds())
        .below(&["vfs"], riot_coap_handler_demos::vfs::vfs(""))
        .below(
            &["saul"],
            riot_coap_handler_demos::saul::SaulHandler::new(&["saul"]),
        )
        .below(&["netif"], riot_coap_handler_demos::netif::netif())
        .below(
            &["nib", "neigh"],
            riot_coap_handler_demos::nib::neighbor_cache(),
        )
        .below(&["ping"], riot_coap_handler_demos::ping::ping_tree(&PINGS))
        .at(
            &["pinged"],
            riot_coap_handler_demos::ping_passive::resource(&PING_PASSIVE),
        )
        .with_wkc();
    let mut handler = riot_wrappers::coap_handler::v0_2::GcoapHandler(handler);

    let mut listener = gcoap::SingleHandlerListener::new_catch_all(&mut handler);

    static SLOT: static_cell::StaticCell<
        gnrc::netreg::callback::Slot<riot_coap_handler_demos::ping::PingCallback>,
    > = static_cell::StaticCell::new();
    PINGS.register(SLOT.init(Default::default()));

    static PASSIVE_SLOT: static_cell::StaticCell<
        gnrc::netreg::callback::Slot<
            &'static riot_coap_handler_demos::ping_passive::PingHistoryMutex<
                { riot_coap_handler_demos::ping_passive::DEFAULT_SIZE },
            >,
        >,
    > = static_cell::StaticCell::new();
    PING_PASSIVE.register(PASSIVE_SLOT.init(Default::default()));

    gcoap::scope(|greg| {
        greg.register(&mut listener);

        println!(
            "CoAP server ready; waiting for interfaces to settle before reporting addresses..."
        );

        let sectimer = ztimer::Clock::sec();
        sectimer.sleep(ztimer::Ticks(2));

        for netif in gnrc::Netif::all() {
            println!(
                "Active interface from PID {:?} ({:?})",
                netif.pid(),
                netif.pid().get_name().unwrap_or("unnamed")
            );
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
        loop {
            PINGS.tick();
            sectimer.sleep(ztimer::Ticks(1));
        }
    })
}
