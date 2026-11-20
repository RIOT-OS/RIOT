#![no_std]

use riot_wrappers::println;
use riot_wrappers::riot_main;

riot_main!(main);

fn main() -> ! {
    // Could also use static-on-stack crate instead
    static EXECUTOR: static_cell::StaticCell<embassy_executor_riot::Executor> =
        static_cell::StaticCell::new();
    let executor: &'static mut _ = EXECUTOR.init(embassy_executor_riot::Executor::new());
    executor.run(|spawner| {
        spawner
            .spawn(amain(spawner))
            .expect("Task did not get spawned before");
    })
}

#[embassy_executor::task]
async fn amain(spawner: embassy_executor::Spawner) {
    use riot_wrappers::ztimer::*;

    let msec = Clock::msec();

    let locked = msec.acquire();

    println!("Waiting 500 ticks on the msec timer before doing anything else");
    // Locking and taking before/after is a bit crude, but the `.time()` method is not yet
    // available for asynchronous closures.
    let before = locked.now();
    msec.sleep_async(Ticks(500)).await;
    let after = locked.now();
    println!(
        "That took us from {:?} to {:?}, which is {} ticks.",
        before,
        after,
        (after - before).0
    );
    drop(locked);
    println!("And now for something more complex...");

    spawner
        .spawn(ten_tenths())
        .expect("Task did not get spawned before");
    spawner
        .spawn(five_fifths())
        .expect("Task did not get spawned before");
}

#[embassy_executor::task]
async fn ten_tenths() {
    use riot_wrappers::ztimer::*;

    println!("A: Will wake up 10x in 1s");
    for _ in 0..10 {
        Clock::msec().sleep_async(Ticks(100)).await;
        println!("A: tick");
    }
    println!("A: Done");
}

#[embassy_executor::task]
async fn five_fifths() {
    use riot_wrappers::ztimer::*;

    println!("B: Will wake up 5x in 1s");
    for _ in 0..5 {
        Clock::msec().sleep_async(Ticks(200)).await;
        println!("B: tick");
    }
    println!("B: Done");
}
