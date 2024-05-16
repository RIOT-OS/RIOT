use riot_wrappers::println;
use embassy_executor::{Spawner, raw::Executor as RawExecutor};
use super::{to_raw, static_from_raw};

#[export_name = "__pender"]
fn pender(context: *mut ()) {
    let signaler: &'static Signaler = static_from_raw(context);
    if 1 == 1 { println!("@@ pender(): signaler: {:?}", signaler); }
}

pub struct Executor {
    executor: RawExecutor,
    _signaler: Signaler,
}

#[derive(Debug)]
struct Signaler(());

impl Executor {
    pub fn new() -> Self {
        let mut signaler = Signaler(());

        Self {
            executor: RawExecutor::new(to_raw(&mut signaler)),
            _signaler: signaler,
        }
    }

    pub fn run(&'static mut self, init: impl FnOnce(Spawner)) -> ! {
        init(self.executor.spawner());

        loop {
            unsafe { self.executor.poll() };
        }
    }
}