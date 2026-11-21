//! Interaction with interrupts
//!
//! The RIOT wrappers offer two ways to interact with interrupts:
//!
//! * Utility functions can disable interrupts (creating critical sections), check whether
//!   interrupts are enabled or to determine whether code is executed in a thread or an ISR.
//!
//! * Some functions (eg. [set_during](crate::thread::ValueInThread<crate::ztimer::Clock>::set_during))
//!   take callbacks that will be called in an interrupt context.
//!
//!   These are typechecked to be Send, as they are moved from the thread to the interrupt context.
//!
//! Not provided by riot-wrappers are methods of implementing interrupts that are directly called
//! by the CPU's interrupt mechanism. These are `extern "C"` functions (often with a `() -> ()`
//! signature) exported under a particular name using `#[no_mangle]`. Any platform specifics (such
//! as the [`riot_sys::inline::cortexm_isr_end()`] function) need to be managed by the
//! implementer, just as when implementing a C interrupt.
//!
//! Rust code intended for use within interrupts does not generally need special precautions -- but
//! several functions (generally, anything that blocks) are discouraged (as they may fail or stall
//! the system) outside of a thread context, or even "forbidden" (because they reliably lock up the
//! system, such as [crate::mutex::Mutex::lock()]). These functions often have preferred
//! alternatives that can be statically known to be executed in a thread context by keeping a copy
//! of [`crate::thread::InThread`].

/// Trivial safe wrapper for
/// [`irq_is_in`](https://doc.riot-os.org/group__core__irq.html#ga83decbeef665d955290f730125ef0e3f)
///
/// Returns true when called from an interrupt service routine
pub(crate) fn irq_is_in() -> bool {
    unsafe { riot_sys::irq_is_in() }
}

/// Trivial safe wrapper for
/// [`irq_is_enabled`](https://doc.riot-os.org/group__core__irq.html#ga7fa965063ff2f4f4cea34f1c2a8fac25)
///
/// Returns true if interrupts are currently enabled
///
/// Note that this only returns reliable values when called from a thread context.
pub(crate) fn irq_is_enabled() -> bool {
    unsafe { riot_sys::irq_is_enabled() }
}

impl crate::thread::InThread {
    /// Trivial safe wrapper for
    /// [`irq_is_enabled`](https://doc.riot-os.org/group__core__irq.html#ga7fa965063ff2f4f4cea34f1c2a8fac25)
    ///
    /// Returns true if interrupts are currently enabled
    ///
    /// Using this on an `InThread` token is preferred over the global function, as the function
    /// only returns reliable values when called from a thread context.
    pub fn irq_is_enabled(self) -> bool {
        irq_is_enabled()
    }
}

/// Proof of running inside a critical section. Reexported from the [bare_metal] crate.
pub use bare_metal::CriticalSection;

/// Run a closure in the current context, but with interrupts disabled.
///
/// The function gets passed a [`bare_metal::CriticalSection`] attesting to the fact that
/// interrupts are off.
///
/// This is equivalent to the [cortex_m crate function of the same
/// name](https://docs.rs/cortex-m/latest/cortex_m/interrupt/fn.free.html).
#[doc(alias = "irq_disable")]
pub fn free<R, F: FnOnce(&CriticalSection) -> R>(f: F) -> R {
    let stored = unsafe { riot_sys::irq_disable() };

    let cs = unsafe { CriticalSection::new() };

    let ret = f(&cs);

    unsafe { riot_sys::irq_restore(stored) };
    ret
}
