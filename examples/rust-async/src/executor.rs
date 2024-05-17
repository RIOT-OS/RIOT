use riot_wrappers::{println, ztimer};
use embassy_executor::{Spawner, raw::Executor as RawExecutor};
use crate::util::{to_raw, static_from_raw};

#[export_name = "__pender"]
fn pender(context: *mut ()) {
    let signaler: &'static Signaler = static_from_raw(context);
    if 1 == 1 { println!("@@ pender(): signaler: {:?}", signaler); }
}

pub struct Executor {
    executor: RawExecutor,
    _signaler: Signaler,
    throttle: Option<u32>,
}

#[derive(Debug)]
struct Signaler(());

impl Executor {
    pub fn new(throttle: Option<u32>) -> Self {
        let mut signaler = Signaler(());

        Self {
            executor: RawExecutor::new(to_raw(&mut signaler)),
            _signaler: signaler,
            throttle
        }
    }

    pub fn run(&'static mut self, init: impl FnOnce(Spawner)) -> ! {
        init(self.executor.spawner());

        if let Some(ms) = self.throttle {
            println!("Executor::run(): throttle: {:?} ms", ms);
            let timer = ztimer::Clock::msec();

            loop {
                timer.sleep_ticks(ms);
                println!("(throttle={}) calling `executor.poll()`", ms);
                unsafe { self.executor.poll() };
            }
        } else {
            loop {
                unsafe { self.executor.poll() };
            }
        }
    }
}