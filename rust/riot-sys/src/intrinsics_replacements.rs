//! Replacements for [core::intrinsics] to work around <https://github.com/immunant/c2rust/issues/372>
//!
//! They are currently only define on riscv32; other platforms (or functions) can be added when
//! necessary. They can't be defined for all platforms, as Cortex-M0 don't have this operation.
//! (But then again, they're not needed, as theire RIOT C code can't contain these operations).

#[cfg(target_arch = "riscv32")]
use core::sync::atomic::{AtomicU32, Ordering};

#[cfg(target_arch = "riscv32")]
pub(crate) fn atomic_and_relaxed(dst: *mut u32, src: u32) -> u32 {
    let actual_atomic = unsafe { &*(dst as *mut AtomicU32) };
    actual_atomic.fetch_and(src, Ordering::Relaxed)
}

#[cfg(target_arch = "riscv32")]
pub(crate) fn atomic_or_relaxed(dst: *mut u32, src: u32) -> u32 {
    let actual_atomic = unsafe { &*(dst as *mut AtomicU32) };
    actual_atomic.fetch_or(src, Ordering::Relaxed)
}
