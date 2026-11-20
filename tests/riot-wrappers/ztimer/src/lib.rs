#![no_std]

use riot_wrappers::println;
use riot_wrappers::riot_main;

riot_main!(main);

fn main() {
    use riot_wrappers::ztimer::*;

    let msec = Clock::msec();

    println!("Waiting 500 ticks on the msec timer before doing anything else");
    let duration = msec.time(|| {
        msec.sleep(Ticks(500));
    });
    let duration =
        duration.expect("That should not have taken so long that the milliseconds overflowed");
    println!("That took {} ticks", duration.0);
}
