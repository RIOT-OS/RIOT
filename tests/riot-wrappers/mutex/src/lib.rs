#![no_std]

use riot_wrappers::mutex::Mutex;
use riot_wrappers::println;
use riot_wrappers::riot_main;
use riot_wrappers::thread::InThread;

riot_main!(main);

static M1: Mutex<()> = Mutex::new(());

fn main() {
    let l1 = M1.lock();
    drop(l1);
    let m1 = InThread::new().unwrap().promote(&M1);
    let l2 = m1.lock();
    assert!(m1.try_lock().is_none());
    drop(l2);
    let l3 = m1.try_lock();
    assert!(l3.is_some());
    drop(l3);

    let m2 = Mutex::new(0u8);
    *m2.lock() = 4;
    assert!(*m2.lock() == 4);

    println!("SUCCESS");
}
