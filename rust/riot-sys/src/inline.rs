//! # C2Rust transpiled header contents (static inline functions
//!
//! Types in here are distinct from those created in the main module (using bindgen); unifying
//! those will be part of [bindgen's #1334], but it's a long way there.
//!
//! [bindgen's #1334]: https://github.com/rust-lang/rust-bindgen/issues/1344
//!
//! Use these functions through the re-export in the main module, for the C headers may flip-flop
//! between static inline and linked.
//!
//! ---
//!
//! Some special treatment has been applied in the course of the transpilation process:
//!
//! * All functions were made `pub`
//! * All functions have their `extern "C"` removed. Any C component would already use it via their
//!   original definitions, there is no need to re-export them or to restrain their ABI (as they
//!   are here for efficient inlining into Rust code only).
//! * For C const initializers (eg. `#define MUTEX_INIT { { NULL } }`), there is no way for a
//!   transpiler to recognize which type this is actually for. That information is tracked manually
//!   in `build.rs` as a list of known initializers. They get turned into const functions in the
//!   style of `fn init_MUTEX_INIT() -> mutex_t`.
//!
// While it'd be tempting to clean them all up in RIOT by a large constification haul, now is not
// the time for that
#![allow(unused_mut)]
// Probably __attribute__((used)) doesn't get translated
#![allow(unused)]

extern "C" {
    /// Symbol indicating untranslated `llvm_asm!` code.
    ///
    /// When this is missing at the linker stage, do not look for its definition (for it should be
    /// left undefined), but find where it is used and manually translate the assemblies.
    fn llvm_asm_is_not_supported_any_more();
}

/// Compatibility macro that looks up assembly in the deprecated `llvm_asm!` style in a manually
/// manged list of known short snippets mapped to `asm!` equivalents.
///
/// As this is tailored for RIOT's C2Rust output, code not found in the list is not rejected at
/// compile time, but mapped to the external (and nonexistent) symbol
/// [llvm_asm_is_not_supported_any_more]. This allows otherwise unused code (which is there either
/// because the bulk-used CMSIS just defines it and it is unused by RIOT, or just because it
/// doesn't happen to be used by any actually used function) to just slip through without causing
/// much fuss.
///
/// This is defined right in the inline module, because using it from another module would make the
/// imports ambiguous versus the builtin `llvm_asm!` macro.
macro_rules! llvm_asm {
    // They can probably be deduplicated (eg. around known strings like "cpsid i" and "cpsie i"
    // that all just need to be passed on, or by the MSR/MRS generalizing over the service registe)
    // -- but that requires advanced macro magic, and for the current number this does fine. The
    // "memory" clobber is probably just a pessimistic assumption (none of the operation appears to
    // actually clobber anything). Unlike in LLVM, new assembly being volatile is default in new
    // asm (as it's not marked pure, IIUC).
    ("MRS $0, ipsr" : "=r" ($result:ident) : : : "volatile") => {
        core::arch::asm!("MRS {}, ipsr", out(reg) $result);
    };
    // (The following are typically found in programs that use riot_wrappers::interrupt::free or
    // anyting else that toggles interrupts).
    ("MRS $0, primask" : "=r" ($result:ident) : : "memory" : "volatile") => {
        core::arch::asm!("MRS {}, primask", out(reg) $result);
    };
    ("MSR primask, $0" : : "r" ($primask_in:ident) : "memory" : "volatile") => {
        core::arch::asm!("MSR primask, {}", in(reg) $primask_in);
    };
    ("cpsid i" : : : "memory" : "volatile") => {
        core::arch::asm!("cpsid i");
    };
    ("cpsie i" : : : "memory" : "volatile") => {
        core::arch::asm!("cpsie i");
    };
    // From RISC-V's interrupt handling
    ("csrrc $0, mstatus, $1" : "=r" ($state:ident) : "i" ($constant:ident) : "memory" : "volatile") => {
        core::arch::asm!("csrrc {}, mstatus, {}", out(reg) $state, in(reg) $constant);
    };
    ("csrw mstatus, $0" : : "r" ($state:ident) : "memory" : "volatile") => {
        core::arch::asm!("csrw mstatus, {}", in(reg) $state);
    };
    ("csrr $0, mstatus" : "=r" ($state:ident) : : "memory" : "volatile") => {
        core::arch::asm!("csrr {}, mstatus", out(reg) $state);
    };
    ($($x:tt)*) => {{
        llvm_asm_is_not_supported_any_more();
        unreachable!()
    }};
}

use core::ffi as libc;

use c2rust_bitfields::*;

// This is a replacement for the `pub type __locale_t` and the IO lines that C2Rust generates
// because of something from stdlib; it is stripped out of the compiled code and turned into a u8
// pointer for lack of better ideas. (Leaving it as a pub struct would require unstable Rust).
//
// The type is hugely sized to ensure that things crash (or preferably don't build) if at any point
// Rust code tries to touch an instance of it, eg. by allocating one on the stack or statically.
#[cfg(not(feature = "keep-extern-types"))]
include!(concat!(env!("OUT_DIR"), "/pubtype_replacements.rs"));

include!(concat!(env!("OUT_DIR"), "/riot_c2rust_replaced.rs"));
