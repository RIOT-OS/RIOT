//! This module implements critical_section using RIOT's irq_disable()/_restore()
#![cfg(feature = "provide_critical_section_1_0")]

use critical_section::RawRestoreState;

struct CriticalSection;
critical_section::set_impl!(CriticalSection);

unsafe impl critical_section::Impl for CriticalSection {
    unsafe fn acquire() -> RawRestoreState {
        // If this fails to compile (because Rust-on-RIOT has gained support for non-32bit
        // architectures), by that time hopefully critical-section > 1.1.2 has been released, which
        // has restore-state-usize. Just increment the dependency version and set its feature from
        // restore-state-u32 to restore-state-usize.
        unsafe { riot_sys::irq_disable() }
    }

    unsafe fn release(token: RawRestoreState) {
        unsafe { riot_sys::irq_restore(token) };
    }
}
