//! # [ztimer high level timer](https://doc.riot-os.org/group__sys__ztimer.html)
//!
//! ZTimer clocks are usually obtained by calling constructors that depend on the presence of
//! global clocks -- [Clock::sec], [Clock::msec] and [Clock::usec].
//!
//! The methods usable on the clocks typically take durations in the form of [Ticks], which ensure
//! that time calculations are done early but can't be mixed up between clocks. The sleep and spin
//! methods take numeric tick counts and durations, not only for historical reasons, but also
//! because sleeping for a Duration works infallibly (even if the duration exceeds the maximum
//! number of ticks a timer can sleep) by sleeping in repetitions.

#[cfg(riot_module_ztimer_periodic)]
pub mod periodic;

use core::convert::TryInto;
use core::mem::ManuallyDrop;
use core::pin::Pin;

use pin_project::{pin_project, pinned_drop};

use riot_sys::ztimer_clock_t;

use crate::thread::{InThread, ValueInThread};

// Useful for working with durations
const NANOS_PER_SEC: u32 = 1_000_000_000;

/// A clock that knows about its frequency. The pulse length is not given in [core::time::Duration]
/// as that's not yet supported by const generics, and because clock rates are often easier to
/// express in Hertz than in multiples of 10^-n seconds.
#[derive(Copy, Clone)]
pub struct Clock<const HZ: u32>(*mut ztimer_clock_t);

/// A [Clock] that has been acquired using [Clock::acquire()] (which is backed by
/// [ztimer_acquire]). Times from a single acquired clock can be compared.
///
/// While time stamps from that clock are protected against cross-frequency comparison, it is up to
/// the user to not mix time stamps from different clocks that happen to have the same frequency,
/// from different times of the timer being locked, and to ensure that wraparounds are considered.
/// While the former two could be addressed by giving this type and its ticks a brand lifetime, the
/// wraparound issue would not be addressed by that anyway.
///
/// [ztimer_acquire]: (https://doc.riot-os.org/group__sys__ztimer.html#gaaff51039476f11e6969da09493e7ccb0).
pub struct LockedClock<const HZ: u32>(Clock<HZ>);

/// A duration on a clock of fixed speed
///
/// In memory, these are numbers of ticks. Semantically, these are durations of `self.0 / HZ`
/// seconds.
#[derive(Copy, Clone, Debug)]
pub struct Ticks<const HZ: u32>(pub u32);

/// A time on some clock ticking at a fixed speed
///
/// It is up to the user to not compare time stamps from different clocks that tick at the same
/// speed, to handle wraparounds, and to ensure that the clock stayed acquired all the time between
/// the time stamps' acquisitions.
#[derive(Copy, Clone, Debug)]
pub struct Timestamp<const HZ: u32>(pub u32);

impl<const HZ: u32> ValueInThread<Clock<HZ>> {
    /// Pause the current thread for the duration of ticks in the timer's time scale.
    ///
    /// Wraps [ztimer_sleep](https://doc.riot-os.org/group__sys__ztimer.html#gade98636e198f2d571c8acd861d29d360)
    #[doc(alias = "ztimer_sleep")]
    pub fn sleep(&self, duration: Ticks<HZ>) {
        unsafe { riot_sys::ztimer_sleep(self.0, duration.0) };
    }

    /// Keep the current thread in a busy loop until the duration of ticks in the timer's tim scale
    /// has passed
    ///
    /// Quoting the original documentation, "This blocks lower priority threads. Use only for
    /// *very* short delays.".
    ///
    /// Wraps [ztimer_spin](https://doc.riot-os.org/group__sys__ztimer.html#ga9de3d9e3290746b856bb23eb2dccaa7c)
    ///
    /// Note that this would not technically require the self to be a [ValueInThread] (as spinning
    /// is doable in an ISR), but it's so discouraged that the Rust wrapper takes the position that
    /// it's best done using a [ValueInThread].
    #[doc(alias = "ztimer_spin")]
    pub fn spin(&self, duration: Ticks<HZ>) {
        unsafe { riot_sys::ztimer_spin(crate::inline_cast_mut(self.0), duration.0) };
    }

    /// Pause the current thread for the given duration, possibly exceeding values expressible in
    /// [`Ticks<HZ>`].
    ///
    /// The duration is converted into ticks (rounding up), and overflows are caught by sleeping
    /// multiple times.
    ///
    /// It is up to the caller to select the Clock suitable for efficiency. (Even sleeping for
    /// seconds on the microseconds timer would not overflow the timer's interface's u32, but the
    /// same multiple-sleeps trick may need to be employed by the implementation, *and* would keep
    /// the system from entering deeper sleep modes).
    pub fn sleep_extended(&self, duration: core::time::Duration) {
        // Convert to ticks, rounding up as per Duration documentation
        let mut ticks = (duration * HZ - core::time::Duration::new(0, 1)).as_secs() + 1;
        while ticks > u32::MAX.into() {
            self.sleep(Ticks(u32::MAX));
            ticks -= u64::from(u32::MAX);
        }
        self.sleep(Ticks(
            ticks.try_into().expect("Was just checked manually above"),
        ));
    }

    /// Set the given callback to be executed in an interrupt some ticks in the future.
    ///
    /// Then, start the in_thread function from in the thread this is called from (as a regular
    /// function call).
    ///
    /// After the in_thread function terminates, the callback is dropped if it has not already
    /// triggered.
    ///
    /// Further Development:
    ///
    /// * This could probably be done with some sort of pinning instead, thus avoiding the nested
    ///   scope -- but getting the Drop right is comparatively tricky, because when done naively it
    ///   needs runtime state.
    ///
    /// * The callback could be passed something extra that enables it to set the timer again and
    ///   again. Granted, there's ztimer_periodic for these cases (and it has better drifting
    ///   properties), but for something like exponential retransmission it could be convenient.
    ///
    ///   (Might make sense to do this without an extra function variant: if the callback ignores
    ///   the timer argument and always returns None, that's all in the caller type and probebly
    ///   inlined right away).
    ///
    /// While (unless with sleep) nothing would break if this were called from an interrupt
    /// context, it would not work either: As RIOT uses flat interrupt priorities, any code
    /// executed in the `in_thread` handler would still be run in the original interrupt, and while
    /// the configured ZTimer would fire its interrupt during that time, the interrupt would not be
    /// serviced, and the timer would be removed already by the time the original interrupt
    /// completes and ZTimer is serviced (finding no actually pending callback).
    pub fn set_during<I: FnOnce() + Send, M: FnOnce() -> R, R>(
        &self,
        callback: I,
        ticks: Ticks<HZ>,
        in_thread: M,
    ) -> R {
        self.into_inner().set_during(callback, ticks, in_thread)
    }
}

impl<const HZ: u32> Clock<HZ> {
    /// Similar to [`.sleep()`], but this does not block but creates a future to be
    /// `.await`ed.
    ///
    /// Note that time starts running only when this is polled, for otherwise there's no pinned
    /// Self around.
    pub async fn sleep_async(&self, duration: Ticks<HZ>) {
        AsyncSleep::NeverPolled(NascentAsyncSleep {
            clock: *self,
            ticks: duration,
        })
        .await
    }

    /// A `ztimer_now()` wrapper that is not public because there needs to be a reason why the
    /// result makes sense, which can come for example from an acquisition.
    fn now(&self) -> Timestamp<HZ> {
        // The `as u32` strips down the 64bit value of the deprecated ZTIMER_NOW64
        Timestamp(unsafe { riot_sys::inline::ztimer_now(crate::inline_cast_mut(self.0)) } as u32)
    }

    /// A version of [`ValueInThread<Clock>::set_during`] that relies on this module's knowledge of
    /// the circumstances to state the validity of its use even without a [`ValueInThread`]
    fn set_during<I: FnOnce() + Send, M: FnOnce() -> R, R>(
        &self,
        callback: I,
        ticks: Ticks<HZ>,
        in_thread: M,
    ) -> R {
        use core::cell::UnsafeCell;

        // This is zero-initialized, which is the more efficient mode for ztimer_t.
        let mut timer = riot_sys::ztimer_t::default();

        // FIXME: If we were worried about what this does during unwind, we might put a Drop on a
        // type around this. (But currently, Rust on RIOT does not unwind).
        //
        // As this is later put into timer.arg, this will need to stay put now (but we can't
        // directly Pin<&mut> it because we need ownership for the FnOnce)
        //
        // * ManuallyDrop because by the time we're done with it it may or may not have already been
        //   dropped.
        // * UnsafeCell because it may be mutaged in the ISR (although if it does get mutated, we're
        //   not touching it any more, so that mightbe overkill).
        let mut callback = UnsafeCell::new(ManuallyDrop::new(callback));

        // Under the stacked borrows model, that's the SharedReadWrite baseline everybody builds on
        // and nobody drops.
        let callback: *mut _ = &mut callback;

        extern "C" fn caller<I: FnOnce() + Send>(arg: *mut crate::libc::c_void) {
            // unsafe: Was cast from the same type when assigned to arg.
            //
            // Reference construction: We're in a critical section, and the main thread only holds
            // the *mut that this was derived from (so under the stacked borrows model, we pop down
            // to that but there's nothing removed).
            let callback: &mut UnsafeCell<ManuallyDrop<I>> =
                unsafe { &mut *(arg as *mut UnsafeCell<ManuallyDrop<I>>) };
            // unsafe: The other take (actually drop) coordinates through the ztimer return value,
            // so that only one of these is ever run.
            let taken = unsafe { ManuallyDrop::take(callback.get_mut()) };
            taken();
        }

        timer.callback = Some(caller::<I>);
        timer.arg = callback as *mut _;

        // Placed in an UnsafeCell because while it is here it may get mutated inside an ISR
        let timer = UnsafeCell::new(timer);

        // unsafe: OK per C API
        unsafe {
            riot_sys::ztimer_set(self.0, timer.get(), ticks.0);
        }

        let result = in_thread();

        // unsafe: OK per C API
        let removed = unsafe { riot_sys::ztimer_remove(self.0, timer.get()) };

        if removed {
            // unsafe: removed == true means that the other drop (actually take) has not been run
            //
            // Reference construction: OK because while the IRQ has fired (and built on the shared
            // base), it has run to completion already and doesn't need its stack items any more.
            unsafe {
                ManuallyDrop::drop((&mut *callback).get_mut());
            }
        }

        result
    }

    /// Keep the clock being shut down or reset for low power modes
    ///
    /// While the clock is locked, its [`LockedClock::now()`] method is available, and its values
    /// can be compared.
    #[doc(alias = "ztimer_acquire")]
    pub fn acquire(&self) -> LockedClock<HZ> {
        // ztimer_acquire is inline or non-inline depending on ZTIMER_ONDEMAND
        #[allow(unused_imports)] // reason: which of these is used depends on ZTIMER_ONDEMAND
        use riot_sys::inline::*;
        #[allow(unused_imports)] // reason: which of these is used depends on ZTIMER_ONDEMAND
        use riot_sys::*;

        // ztimer_acquire takes a mut even though ztimer itself takes care of synchronization
        let clock = self.0 as *mut riot_sys::ztimer_clock_t;

        // unsafe: C function can be called at any time
        unsafe { ztimer_acquire(crate::inline_cast_mut(clock)) };

        LockedClock(self.clone())
    }

    /// Run a closure and measure the time it takes.
    ///
    /// If the time the closure took exceeded the 2³²-1 ticks (the maximum time measurable on that
    /// clock), None is returned.
    #[doc(alias = "ztimer_stopwatch")]
    pub fn time(&self, closure: impl FnOnce()) -> Option<Ticks<HZ>> {
        self.time_with_result(closure).0
    }

    /// Like [`Self::time()`], but allowing the closure to return a value.
    ///
    /// As an implementation note, this is not using `ztimer_stopwatch` because that can not detect
    /// overflows; if overflow detection is added to `ztimer_stopwatch` later, the implementation
    /// can change.
    pub fn time_with_result<R>(&self, closure: impl FnOnce() -> R) -> (Option<Ticks<HZ>>, R) {
        // There is a more efficient implementation of this than set_during that looks at the result
        // of ztimer_remove, but I'm lazy today.
        //
        // FIXME: Implement it more efficiently.

        let mut fired = false;

        // We're faking being in a thread here because generally, set_during only makes sense in a
        // thread context. As the closure is being run in an interrupt, we already accept that
        // blocking for longer than the shortest ZTimer's wrapping time subtly breaks ZTimer --
        // that's a limitation of ZTimer and our interrupts. When used in an interrupt, this
        // function does needless work of setting and removing the timer, but it is not wrong to
        // use it, and if the function being timed doesn't already break ZTimer, the result is
        // valid.
        let (before, result, after) = self.set_during(
            || fired = true,
            Ticks(u32::MAX),
            || {
                let before = self.now();
                let result = closure();
                let after = self.now();
                (before, result, after)
            },
        );

        let time = if fired { None } else { Some(after - before) };

        (time, result)
    }
}

impl<const HZ: u32> LockedClock<HZ> {
    /// Get the current time value of the clock.
    #[doc(alias = "ztimer_now")]
    pub fn now(&self) -> Timestamp<HZ> {
        self.0.now()
    }
}

impl<const HZ: u32> Drop for LockedClock<HZ> {
    fn drop(&mut self) {
        // ztimer_release is inline or non-inline depending on ZTIMER_ONDEMAND
        #[allow(unused_imports)] // reason: which of these is used depends on ZTIMER_ONDEMAND
        use riot_sys::inline::*;
        #[allow(unused_imports)] // reason: which of these is used depends on ZTIMER_ONDEMAND
        use riot_sys::*;

        // ztimer_release takes a mut even though ztimer itself takes care of synchronization
        let clock = self.0 .0 as *mut riot_sys::ztimer_clock_t;

        // unsafe: C function can be called at any time
        unsafe { ztimer_acquire(crate::inline_cast_mut(clock)) };
    }
}

impl<const HZ: u32> core::ops::Sub for Timestamp<HZ> {
    type Output = Ticks<HZ>;

    fn sub(self, other: Self) -> Ticks<HZ> {
        Ticks(self.0.wrapping_sub(other.0))
    }
}

impl Clock<1> {
    /// Get the global second ZTimer clock, ZTIMER_SEC.
    ///
    /// This function verifies (at a small runtime cost) that the caller is in a thread context.
    /// This can be avoided by calling `in_thread.promote(Clock::sec_unbound())` on an existing
    /// [crate::thread::InThread] token.
    #[cfg(riot_module_ztimer_sec)]
    #[doc(alias = "ZTIMER_SEC")]
    pub fn sec() -> ValueInThread<Self> {
        InThread::new()
            .expect("Thread-bound ZTimer clock created in ISR")
            .promote(Self::sec_unbound())
    }

    /// Get the global second ZTimer clock, ZTIMER_SEC.
    ///
    /// The clock is *not* packed in a [ValueInThread], which makes the blocking sleep methods and
    /// delay implementations unavailable, but works even in interrupts contexts.
    #[cfg(riot_module_ztimer_sec)]
    pub fn sec_unbound() -> Self {
        Clock(unsafe { riot_sys::ZTIMER_SEC })
    }
}

impl Clock<1000> {
    /// Get the global milliseconds ZTimer clock, ZTIMER_MSEC.
    ///
    /// This function verifies (at a small runtime cost) that the caller is in a thread context.
    /// This can be avoided by calling `in_thread.promote(Clock::msec_unbound())` on an existing
    /// [crate::thread::InThread] token.
    #[cfg(riot_module_ztimer_msec)]
    #[doc(alias = "ZTIMER_MSEC")]
    pub fn msec() -> ValueInThread<Self> {
        InThread::new()
            .expect("Thread-bound ZTimer clock created in ISR")
            .promote(Self::msec_unbound())
    }

    /// Get the global milliseconds ZTimer clock, ZTIMER_MSEC.
    ///
    /// The clock is *not* packed in a [ValueInThread], which makes the blocking sleep methods and
    /// delay implementations unavailable, but works even in interrupts contexts.
    #[cfg(riot_module_ztimer_msec)]
    pub fn msec_unbound() -> Self {
        Clock(unsafe { riot_sys::ZTIMER_MSEC })
    }
}

impl Clock<1000000> {
    /// Get the global microseconds ZTimer clock, ZTIMER_USEC.
    ///
    /// This function verifies (at a small runtime cost) that the caller is in a thread context.
    /// This can be avoided by calling `in_thread.promote(Clock::usec_unbound())` on an existing
    /// [crate::thread::InThread] token.
    #[cfg(riot_module_ztimer_usec)]
    #[doc(alias = "ZTIMER_USEC")]
    pub fn usec() -> ValueInThread<Self> {
        InThread::new()
            .expect("Thread-bound ZTimer clock created in ISR")
            .promote(Self::usec_unbound())
    }

    /// Get the global microseconds ZTimer clock, ZTIMER_USEC.
    ///
    /// The clock is *not* packed in a [ValueInThread], which makes the blocking sleep methods and
    /// delay implementations unavailable, but works even in interrupts contexts.
    #[cfg(riot_module_ztimer_usec)]
    pub fn usec_unbound() -> Self {
        Clock(unsafe { riot_sys::ZTIMER_USEC })
    }
}

#[cfg(all(feature = "embedded-hal-async", riot_module_ztimer_usec))]
/// Struct that provides the [embedded_hal_async::delay::DelayNs] trait
///
/// Unlike the [Clock] structs that can be instantiated for any ZTimer clock, this is clock
/// independent, because the embedded HAL trait offers delay methods that are provided through
/// different global clocks.
///
/// ## Caveats
///
/// RIOT does not provide a general nanosecond clock; nanosecond sleeps are implemented at the
/// microsecond clock, and will pause longer as the trait demands.
#[derive(Copy, Clone, Debug)]
pub struct Delay;

#[cfg(all(
    feature = "embedded-hal-async",
    riot_module_ztimer_usec,
    riot_module_ztimer_msec
))]
impl embedded_hal_async::delay::DelayNs for Delay {
    async fn delay_ns(&mut self, ns: u32) {
        // See struct level documentation
        Clock::usec_unbound()
            .sleep_async(Ticks(ns.div_ceil(1000)))
            .await
    }

    async fn delay_us(&mut self, us: u32) {
        Clock::usec_unbound().sleep_async(Ticks(us)).await
    }

    async fn delay_ms(&mut self, us: u32) {
        Clock::msec_unbound().sleep_async(Ticks(us)).await
    }
}

impl<const F: u32> embedded_hal::delay::DelayNs for ValueInThread<Clock<F>> {
    // FIXME: Provide delay_us and delay_ms, at least for the clocks where those fit, to avoid the
    // loops where the provided function wakes up every 4.3s

    #[inline(always)]
    fn delay_ns(&mut self, ns: u32) {
        if F > NANOS_PER_SEC {
            // On really fast ZTimers, we may need to loop (but let's implement this when anyone
            // ever implements a faster-than-nanosecond timer)
            todo!("Test for whether this needs to loop")
        } else {
            // No need to loop, but we need to take care not to overflow -- and we can't
            // pre-calculate (F / NANOS_PER_SEC) because that's rounded to 0

            // FIXME: There has to be a more efficient way -- for now we're relying on inlining and
            // hope that constant propagation takes care of things

            // FIXME: This does not round correctly (it should round up the ticks), but ztimer
            // ticks have some uncertainty on their own anyway.

            let ticks = (ns as u64) * (F as u64) / (NANOS_PER_SEC as u64);
            self.sleep(Ticks(ticks as u32));
        }
    }
}

/// The error type of fallible conversions to ticks.
///
/// Overflow is the only ever indicated error type; lack of accuracy in the timer does not
/// constitute a reportable error, and is always resolved by rounding up (consistent with ZTimer's
/// and Duration's behavior).
#[derive(Debug)]
pub struct Overflow;

impl<const HZ: u32> Ticks<HZ> {
    /// Maximum duration expressible on a clock with the given frequency
    pub const MAX: Self = Ticks(u32::MAX);

    /// Fallible conversion from a Duration
    ///
    /// This is an extra function (equivalently available as try_from) as it allows the result to
    /// be const (which many constructed durations are).
    ///
    /// Conversion is not perfect if HZ does not a divisor of $10^9$.
    ///
    /// This will be deprecated when TryFrom / TryInto can be optionally const (see
    /// <https://github.com/rust-lang/rust/issues/67792> for efforts).
    /*
    pub fn from_duration(duration: core::time::Duration) -> Result<Self, Overflow> {
        // Manual div_ceil while that's unstable, see
        // <https://github.com/rust-lang/rust/issues/88581>
        let subsec_ticks = match duration.subsec_nanos() {
            0 => 0,
            n => (n - 1) / (NANOS_PER_SEC / HZ) + 1
        };
        u32::try_from(duration.as_secs())
            .ok()
            .and_then(|s| s.checked_mul(HZ))
            .and_then(|t| t.checked_add(subsec_ticks))
            .map(|t| Ticks(t))
            .ok_or(Overflow)
    }
    */
    // Edited from the above until and_then & co are usable for const functions
    pub const fn from_duration(duration: core::time::Duration) -> Result<Self, Overflow> {
        // Manual div_ceil while that's unstable, see
        // <https://github.com/rust-lang/rust/issues/88581>
        let subsec_ticks = match duration.subsec_nanos() {
            0 => 0,
            n => (n - 1) / (NANOS_PER_SEC / HZ) + 1,
        };
        let secs = duration.as_secs();
        if secs > u32::MAX as _ {
            return Err(Overflow);
        };
        let secs = secs as u32;
        let sec_ticks = match secs.checked_mul(HZ) {
            Some(s) => s,
            _ => return Err(Overflow),
        };
        let sum_ticks = match sec_ticks.checked_add(subsec_ticks) {
            Some(s) => s,
            _ => return Err(Overflow),
        };
        Ok(Ticks(sum_ticks))
    }
}

impl<const HZ: u32> TryFrom<core::time::Duration> for Ticks<HZ> {
    type Error = Overflow;

    fn try_from(duration: core::time::Duration) -> Result<Self, Overflow> {
        Self::from_duration(duration)
    }
}

#[derive(Copy, Clone)]
struct NascentAsyncSleep<const HZ: u32> {
    clock: crate::ztimer::Clock<HZ>,
    ticks: Ticks<HZ>,
}

#[pin_project(PinnedDrop)]
struct RunningAsyncSleep<const HZ: u32> {
    clock: crate::ztimer::Clock<HZ>,

    #[pin]
    timer: riot_sys::ztimer_t,
    // If this only were pointer-sized, it'd fit inside the ztimer and we wouldn't have to lug
    // it around -- but it isn't, and it looks like we don't get it scaled down easily (that
    // is, without patching core to only accept a very specific kind of wakers).
    //
    // This is initialized at construction time, and gets consumed either at callback time or at
    // drop time.
    #[pin]
    waker: ManuallyDrop<core::task::Waker>,

    #[pin]
    // riot_sys::ztimer_t is Unpin because riot-sys doesn't know any better
    _pin: core::marker::PhantomPinned,
}

#[pin_project(project=ProjectedAsyncSleep)]
enum AsyncSleep<const HZ: u32> {
    NeverPolled(NascentAsyncSleep<HZ>),
    Running(#[pin] RunningAsyncSleep<HZ>),
}

impl<const HZ: u32> core::future::Future for AsyncSleep<HZ> {
    type Output = ();

    fn poll(mut self: Pin<&mut Self>, ctx: &mut core::task::Context<'_>) -> core::task::Poll<()> {
        // There's no unsafe version of set_during, thus emulating this ourselves
        //
        // This should be conceptually sound: The timer gets set, and a cloned waker gets moved in.
        // The timer itself is pinned and thus won't move away without a Drop, and the moved in
        // argument is owned (here it being pinned might not be enough, as it's used as a callback,
        // and then everything accessible from the callback would need to be pinned as well, just
        // in case the timer went out of lifetime without being dropped, which is OK as long as
        // it's never accessed, and thus we may only access memory from there, probably ... the DMA
        // problem).

        // To use the data in nascent we have to keep &mut self usable; cloning this out is more
        // about making the borrow checker happy: It doesn't see that when clocks and ticks are
        // moved out of nascent, the lifetime of the `match self.as_mut().project()` value can be
        // terminated alreadyh before we write to &mut self again.
        let copied_out = match self.as_mut().project() {
            ProjectedAsyncSleep::NeverPolled(nascent) => Some(nascent.clone()),
            _ => None,
        };

        if let Some(nascent) = copied_out {
            let NascentAsyncSleep { clock, ticks } = nascent;

            let mut timer: riot_sys::ztimer_t = Default::default();
            extern "C" fn wake_arg(arg: *mut crate::libc::c_void) {
                // Moving it out of its pinned position, leaving the bit pattern in place (but it
                // won't ever be used again, as the timer only fires once).
                let waker: core::task::Waker = unsafe { (arg as *mut core::task::Waker).read() };
                waker.wake();
            }
            timer.callback = Some(wake_arg);
            let running = RunningAsyncSleep {
                clock,
                timer,
                waker: ManuallyDrop::new(ctx.waker().clone()),
                _pin: Default::default(),
            };

            Pin::set(&mut self, AsyncSleep::Running(running));

            // Pinned now, can add self referentiality to waker

            let mut running = match self.as_mut().project() {
                ProjectedAsyncSleep::Running(w) => w,
                _ => unreachable!("Was just set to be running"),
            };

            // We're casting a ManuallyDrop into the c_void here and cast it back into a Waker, but
            // that's OK because ManuallyDrop is repr(transparent)
            let waker_address = &running.waker as *const ManuallyDrop<core::task::Waker>
                as *const crate::libc::c_void;
            running.as_mut().project().timer.arg = waker_address as *mut _;
            let timer = &running.timer as *const _ as *mut _;

            // Start timer

            // unsafe: OK per C API
            unsafe {
                riot_sys::ztimer_set(clock.0, timer, ticks.0);
            }

            core::task::Poll::Pending
        } else {
            let running = match self.project() {
                ProjectedAsyncSleep::Running(running) => running,
                _ => unreachable!("Was just checked to be running"),
            };

            // Instead of doing this relatively costly check, might we instead atomically set a
            // property of the PendingTimer in the callback?
            if unsafe { riot_sys::ztimer_is_set(riot_sys::ZTIMER_MSEC, &running.timer) != 0 } {
                core::task::Poll::Pending
            } else {
                core::task::Poll::Ready(())
            }
        }
    }
}

#[pinned_drop]
impl<const HZ: u32> PinnedDrop for RunningAsyncSleep<HZ> {
    fn drop(self: Pin<&mut Self>) {
        // FIXME: Should we store a third state when this gets Ready, just to spare us going through the
        // ztimer_remove? Might be a good idea, might be just an optimization (that doesn't get us
        // much, for if the timer fired, ztimer_remove can take a shortcut route).

        let mut projected = self.project();

        let was_pending = unsafe {
            riot_sys::ztimer_remove(projected.clock.0, projected.timer.as_mut().get_mut())
        };

        if was_pending {
            unsafe { ManuallyDrop::drop(&mut projected.waker) };
        }
    }
}
