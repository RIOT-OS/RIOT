//! # [Periodic ZTimer API](https://doc.riot-os.org/ztimer_2periodic_8h.html)

use core::marker::PhantomPinned;
use core::mem::MaybeUninit;
use core::pin::Pin;

/// Return value of a periodic callback
#[derive(Copy, Clone, Debug)]
pub enum Behavior {
    // The explicit values should make the into functions trivial
    /// Invoke the callback on the next cycle
    KeepGoing = riot_sys::ZTIMER_PERIODIC_KEEP_GOING as isize,
    /// Stop invoking the callback
    Abort = (riot_sys::ZTIMER_PERIODIC_KEEP_GOING as isize) ^ 1,
}

impl Into<crate::libc::c_int> for Behavior {
    fn into(self) -> crate::libc::c_int {
        match self {
            Behavior::KeepGoing => riot_sys::ZTIMER_PERIODIC_KEEP_GOING as _,
            // "any other value"
            Behavior::Abort => (riot_sys::ZTIMER_PERIODIC_KEEP_GOING as crate::libc::c_int) ^ 1,
        }
    }
}

impl Into<bool> for Behavior {
    fn into(self) -> bool {
        match self {
            Behavior::KeepGoing => true,
            Behavior::Abort => false,
        }
    }
}

/// Callback for a periodic timer
///
/// This is implemented as a trait (rather than Timer taking a callback directly) as to allow
/// interaction with the handler in a critical section in [Timer::alter].
pub trait Handler: Send {
    fn trigger(&mut self) -> Behavior;
}

/// A periodic timer
///
/// This periodic timer is built on a [clock](super::Clock) and configured with a frequency and
/// tick handler.
///
/// It contains the handler and a `ztimer_periodic_t` C struct that then contains the actual timer
/// as well as a reference to the clock. Being self-referential by nature, it is mainly used in
/// pinned form. It can be started and stopped, and stops automatically when dropped.
pub struct Timer<H: Handler, const HZ: u32> {
    // When pinned, this must note move.
    timer: riot_sys::ztimer_periodic_t,
    // When pinned, a reference to this is held in the timer, but the handler itself can be swapped
    // around when the timer is not running or paused.
    //
    // (In a sense, we're treating this place in the struct like a bare_metal::Mutex, but as
    // ZTimer's behavior of turning off all interrupts during execution is not made explicit
    // anywhere by manifesting a CriticalSection, it's just done unsafely here right away -- and
    // anyhow would need additional trickery to get a &mut out of it).

    // FIXME: Should this be inside ... something (UnsafeCell is insufficient, as a &mut to it
    // still implies exclusive access) that disallows assumptions on exclusivity?
    handler: H,
    // From the .start(), timer has an internal reference to the handler
    _phantom: PhantomPinned,
}

impl<H: Handler, const HZ: u32> Timer<H, HZ> {
    pub fn new(clock: super::Clock<HZ>, handler: H, ticks: super::Ticks<HZ>) -> Self {
        let mut timer = MaybeUninit::uninit();

        // Leaving the arg blank for the moment, to be set later when we have a Pin<&mut self>
        //
        // The type is self-referential (.timer.arg is the whole thing again), a property which is
        // restored at start when pinned.
        let timer = unsafe {
            riot_sys::ztimer_periodic_init(
                clock.0,
                timer.as_mut_ptr(),
                Some(Self::callback),
                core::ptr::null_mut(),
                ticks.0,
            );
            timer.assume_init()
        };

        Timer {
            timer,
            handler,
            _phantom: PhantomPinned,
        }
    }

    fn restore_internal_references(&mut self) {
        self.timer.arg = &mut self.handler as *mut _ as *mut _;
        self.timer.timer.arg = &mut self.timer as *mut _ as *mut _;
    }

    pub fn stop(&mut self) {
        unsafe {
            riot_sys::ztimer_periodic_stop(&mut self.timer);
        }
    }

    extern "C" fn callback(arg: *mut crate::libc::c_void) -> bool {
        let handler = unsafe { &mut *(arg as *mut H) };
        handler.trigger().into()
    }

    // Put on hold not only because I can't move the fields out due to the presence of a Drop
    // implementation, but also because how would one get an owned self after the type was pinned?
    //     /// Stop the periodic timer, and return the handler that was in the timer
    //     pub fn to_parts(mut self) -> H {
    //         self.stop();
    //         return self.handler;
    //     }

    /// Obtain a mutable reference to the handler.
    ///
    /// This can be used, for example, to feed data into a handler that is sent out whenever the
    /// timer triggers.
    ///
    /// This is relatively invasive to the system as it creates a critical section (ie. possibly
    /// delaying the execution of the next timer, or even other interrupts). In many cases, the
    /// preferable way to send data to the timer is to use a lock-free data structure.
    // This needs to take a &mut self to avoid nesting, otherwise two code paths could do nested
    // .alter().
    pub fn alter<R, F: FnOnce(&mut H) -> R>(self: &mut Pin<&mut Self>, f: F) -> R {
        crate::interrupt::free(|_| {
            // unsafe: Only accessing handler
            let s = unsafe { Pin::into_inner_unchecked(self.as_mut()) };
            f(&mut s.handler)
        })
    }
}

impl<H: Handler + 'static, const HZ: u32> Timer<H, HZ> {
    #[doc(alias = "ztimer_periodic_start")]
    /// Start the timer, calling the handler at every interval.
    ///
    /// This requires a `Handler + 'static` because it relies on the timer's drop to stop the
    /// process, and only a static handler can still safely be called if that drop never happens.
    ///
    /// (For non-static handlers, a scoped version might be introduced later).
    pub fn start(self: &mut Pin<&mut Self>) {
        unsafe {
            // unsafe: Nothing moved around with these references
            let s = Pin::into_inner_unchecked(self.as_mut());
            s.restore_internal_references();
            // unsafe: C API
            riot_sys::ztimer_periodic_start(&mut s.timer);
        }
    }
}

impl<H: Handler, const HZ: u32> Drop for Timer<H, HZ> {
    fn drop(&mut self) {
        self.stop();
        // and then drop the fields
    }
}
