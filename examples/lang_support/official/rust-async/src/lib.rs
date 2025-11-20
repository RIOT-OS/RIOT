// SPDX-FileCopyrightText: 2020 Christian Amsüss
// SPDX-License-Identifier: LGPL-2.1-only
#![no_std]

use riot_wrappers::println;
use riot_wrappers::riot_main;
use riot_wrappers::ztimer::{Clock, Ticks};

use static_cell::StaticCell;

extern crate rust_riotmodules;

riot_main!(main);

fn main() {
    // Create an executor for running async functions
    static EXECUTOR: StaticCell<embassy_executor_riot::Executor> = StaticCell::new();
    let executor: &'static mut _ = EXECUTOR.init(embassy_executor_riot::Executor::new());
    executor.run(|spawner| {
        spawner
            .spawn(amain(spawner))
            .expect("Starting task for the first time");
    })
}

#[embassy_executor::task]
async fn amain(spawner: embassy_executor::Spawner) {
    println!("Running asynchronously:");

    spawner
        .spawn(fast_messages("A"))
        .expect("Starting task for the first time");
    spawner
        .spawn(fast_messages("B"))
        .expect("Task is configured to allow two instances");

    let msec = Clock::msec();

    msec.sleep_async(Ticks(2500)).await;

    println!("Slept 2.5 seconds in 'amain' task, that should be enough.");
}

#[embassy_executor::task(pool_size = 2)]
async fn fast_messages(taskname: &'static str) {
    println!("Task {taskname} is running:");

    let msec = Clock::msec();

    msec.sleep_async(Ticks(1000)).await;
    println!("Task {taskname} ticked");
    msec.sleep_async(Ticks(1000)).await;
    println!("Task {taskname} tocked");
    msec.sleep_async(Ticks(1000)).await;
    println!("Task {taskname} is done.");
}
