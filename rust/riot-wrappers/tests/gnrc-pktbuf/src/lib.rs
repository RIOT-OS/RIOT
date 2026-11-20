#![no_std]

use riot_wrappers::println;
use riot_wrappers::riot_main;

use riot_wrappers::gnrc_pktbuf::{Pktsnip, Shared};

riot_main!(main);

#[track_caller]
fn check() {
    assert!(
        unsafe { riot_sys::gnrc_pktbuf_is_sane() },
        "GNRC packet buffer in unsound state"
    );
}

#[track_caller]
fn check_empty() {
    check();
    assert!(
        unsafe { riot_sys::gnrc_pktbuf_is_empty() },
        "GNRC packet buffer should be empty at this point"
    );
}

fn main() {
    check_empty();

    let snip1 = Pktsnip::allocate(128, riot_sys::gnrc_nettype_t_GNRC_NETTYPE_UNDEF).unwrap();
    println!("Allocated pktsnip {:?}", snip1);

    check();

    let snip2 =
        Pktsnip::allocate_from(&[1, 2, 3, 4], riot_sys::gnrc_nettype_t_GNRC_NETTYPE_UNDEF).unwrap();
    println!("Allocated another pktsnip {:?}", snip2);
    let snip2 = snip2
        .add(10, riot_sys::gnrc_nettype_t_GNRC_NETTYPE_UNDEF)
        .unwrap();
    println!("... and prefixed it with 10 bytes into {:?}", snip2);

    check();

    let snip2a: Pktsnip<Shared> = snip2.into();
    let snip2b = snip2a.clone();
    let snip2b = snip2b
        .add(10, riot_sys::gnrc_nettype_t_GNRC_NETTYPE_UNDEF)
        .unwrap();
    println!(
        "We have two of them now, the second one with an extension: {:?}, {:?}",
        snip2a, snip2b
    );

    println!(
        "Making the second writable should not change anything about memory usage (not tested)..."
    );
    let snip2b: Pktsnip<Shared> = snip2b.into(); // and the method isn't even available unless we
                                                 // make it shared in the first place
    let snip2b = snip2b.start_write().unwrap();
    println!("... whereas making the first writable entails copying");
    let snip2a = snip2a.start_write().unwrap();
    println!(
        "In the end, they still look the same: {:?}, {:?}",
        snip2a, snip2b
    );

    drop(snip1);
    drop(snip2a);
    drop(snip2b);
    println!("Dropped it");

    check_empty();

    println!("Tests completed.");
}
