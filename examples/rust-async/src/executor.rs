use riot_wrappers::{println, ztimer};
use embassy_executor::{Spawner, raw};
use crate::util::static_from_raw;

#[export_name = "__pender"]
fn pender(context: *mut ()) {
    let signaler: &'static Signaler = static_from_raw(context);
    assert_eq!(signaler.dummy, SIGNALER_DUMMY_DATA);
}

pub struct Executor {
    inner: raw::Executor,
    signaler: &'static Signaler,
    throttle: Option<u32>,
}

use conquer_once::spin::OnceCell;
static SIGNALER: OnceCell<Signaler> = OnceCell::uninit();

struct Signaler {
    dummy: u8,
}

const SIGNALER_DUMMY_DATA: u8 = 42;

impl Signaler {
    fn new() -> Self {
        Self {
            dummy: SIGNALER_DUMMY_DATA,
        }
    }
}

impl Executor {
    pub fn new(throttle: Option<u32>) -> Self {
        SIGNALER.try_init_once(|| Signaler::new()).unwrap();
        let signaler: &'static Signaler = SIGNALER.get().unwrap();

        Self {
            inner: raw::Executor::new(signaler as *const Signaler as *mut ()),
            signaler,
            throttle,
        }
    }

    pub fn run(&'static mut self, init: impl FnOnce(Spawner)) -> ! {
        init(self.inner.spawner());

        assert_eq!(self.signaler.dummy, SIGNALER_DUMMY_DATA);

        if let Some(ms) = self.throttle {
            println!("Executor::run(): throttle: {:?} ms", ms);
            let timer = ztimer::Clock::msec();

            loop {
                timer.sleep_ticks(ms);
                println!("(throttle={}) calling `.poll()`", ms);
                unsafe { self.inner.poll() };
            }
        } else {
            loop {
                unsafe { self.inner.poll() };
            }
        }
    }
}