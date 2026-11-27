#![no_std]

use riot_wrappers::println;
use riot_wrappers::riot_main;

riot_main!(main);

fn main() {
    use riot_wrappers::ztimer::*;

    let msec = Clock::msec();

    loop {
        for netif in riot_wrappers::gnrc::Netif::all() {
            println!(
                "Netif at PID {:?} with link-layer addr {:?}",
                netif.pid(),
                netif.l2addr()
            );
            for addr in &netif.ipv6_addrs().unwrap() {
                println!("- Address {:?}", addr);
            }
        }

        println!("Cache entries:");
        for cache_entry in riot_wrappers::gnrc::nib::NcEntry::all() {
            println!(
                "- on interface {:?}: {:02x?} <=> {:?}, router? {:?}, NUD {:?}, AR {:?}",
                cache_entry.iface(),
                cache_entry.l2addr(),
                cache_entry.ipv6_addr(),
                cache_entry.is_router(),
                cache_entry.nud_state(),
                cache_entry.ar_state()
            );
        }
        println!("");

        msec.sleep(Ticks(300));
    }
}
