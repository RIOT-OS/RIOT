//! Safe and idiomatic Rust wrappers for [RIOT-OS]
//!
//! See [RIOT's documentation on using Rust] for a general introduction to Rust on RIOT, [this
//! crate's README file] on general concepts (such as the interaction between modules here, RIOT
//! modules and features), and the individual modules' documentation entries for details.
//!
//! [RIOT-OS]: https://www.riot-os.org/
//! [RIOT's documentation on using Rust]: https://doc.riot-os.org/using-rust.html
//! [this crate's README file]: https://github.com/RIOT-OS/rust-riot-wrappers

#![no_std]
// Primarily for documentation, see feature docs
#![cfg_attr(feature = "actual_never_type", feature(never_type))]
#![cfg_attr(feature = "nightly_docs", feature(fundamental))]

/// riot-sys is re-exported here as it is necessary in some places when using it to get values (eg.
/// in [error::NumericError::from_constant]). It is also used in macros such as [static_command!].
///
/// ### Stability
///
/// By directly mapping RIOT APIs, this is more volatile than the rest of riot-wrappers. Use this
/// only where necessary to utilize riot-wrappers APIs.
pub use riot_sys;

// When removing, check for occurrences of `$crate::riot_sys::libc` in the modules!
mod libc {
    #[allow(deprecated, reason = "needed while we support riot-sys 0.7")]
    pub use riot_sys::libc::*;
}

/// Re-exporting the cstr macro module because our macros in [shell] use it.
pub use cstr;

pub mod error;

mod helpers;
mod never;

/// The identifier of the RIOT board the program is being built for (`RIOT_BOARD` in C).
#[doc(alias = "RIOT_BOARD")]
pub const BOARD: &'static str = {
    let b = riot_sys::RIOT_BOARD;
    let Ok(b) = core::ffi::CStr::from_bytes_with_nul(b) else {
        // Could be `.expect()`, but that's not const yet
        // Workaround-For: https://github.com/rust-lang/rust/issues/67441
        panic!("Board names are null-terminated C strings");
    };
    let Ok(b) = b.to_str() else {
        panic!("Board names should be ASCII")
    };
    b
};

#[inline]
const fn assert_same_layout<A, B>() {
    const {
        assert!(
            // assert_eq does not work in const yet as of 1.80.1
            core::mem::size_of::<A>() == core::mem::size_of::<B>(),
            "Incompatible pointers between c2rust and bindgen",
            // const_format_args does not support that yet -- but the top line of the error spells
            // out the types.
            // "Incompatible pointers between c2rust and bindgen: {} has size {}, {} has size {}",
            // core::any::type_name::<A>(),
            // core::mem::size_of::<A>(),
            // core::any::type_name::<B>(),
            // core::mem::size_of::<B>(),
        )
    };
}

/// Cast pointers around before passing them in to functions; this is sometimes needed when a
/// struct is used from bindgen (`riot_sys::*`) but passed to a C2Rust function that uses its own
/// definition (`riot_sys::inline::*`).
///
/// Ideally this'd use what comes out of safe transmutation to statically show castability (or not
/// be needed due to better collaboration between C2Rust and bindgen), but until that's a thing,
/// checking for sizes is the least we can do.
///
/// TBD: Make this into a compile time failure (first attempts failed due to "use of generic
/// parameter from outer function" errors). Anyhow, if the check passes, the function essentially
/// becomes a no-op.
#[inline]
fn inline_cast<A, B>(input: *const A) -> *const B {
    assert_same_layout::<A, B>();
    input as _
}

/// `*mut` analogon to [inline_cast]
#[inline]
fn inline_cast_mut<A, B>(input: *mut A) -> *mut B {
    assert_same_layout::<A, B>();
    input as _
}

/// `&` analogon to [inline_cast]
#[inline]
#[allow(unused)]
unsafe fn inline_cast_ref<A, B>(input: &A) -> &B {
    assert_same_layout::<A, B>();
    core::mem::transmute(input)
}

/// `&mut` analogon to [inline_cast]
#[inline]
#[allow(unused)]
unsafe fn inline_cast_ref_mut<A, B>(input: &mut A) -> &mut B {
    assert_same_layout::<A, B>();
    core::mem::transmute(input)
}

#[cfg(riot_module_saul)]
pub mod saul;
#[cfg(riot_module_shell)]
pub mod shell;
pub mod stdio;
pub mod thread;
// internally cfg-gated as it has a no-op implementation
#[cfg(riot_module_gcoap)]
pub mod gcoap;
pub mod gnrc;
// Note that this can also exist without gnrc
#[cfg(riot_module_gnrc_pktbuf)]
pub mod gnrc_pktbuf;
#[cfg(riot_module_gnrc)]
pub mod gnrc_util;
#[cfg(riot_module_periph_i2c)]
pub mod i2c;
#[cfg(riot_module_core_msg)]
pub mod msg;
#[cfg(riot_module_random)]
pub mod random;

#[cfg(riot_module_periph_spi)]
pub mod spi;

#[cfg(riot_module_periph_adc)]
pub mod adc;

#[cfg(riot_module_periph_dac)]
pub mod dac;

#[cfg(riot_module_ztimer)]
pub mod ztimer;

pub mod mutex;
#[cfg(riot_module_pthread)]
pub mod rwlock;

#[cfg(feature = "set_panic_handler")]
mod panic;

#[cfg(riot_module_gcoap)]
pub mod coap_handler;
#[cfg(riot_module_gcoap)]
pub mod coap_message;

#[cfg(riot_module_sock)]
pub mod socket;
#[cfg(all(riot_module_sock_udp, feature = "with_embedded_nal"))]
pub mod socket_embedded_nal;
#[cfg(all(
    riot_module_sock_udp,
    riot_module_sock_aux_local,
    feature = "with_embedded_nal_async"
))]
pub mod socket_embedded_nal_async_udp;
#[cfg(all(riot_module_sock_tcp, feature = "with_embedded_nal"))]
pub mod socket_embedded_nal_tcp;

#[cfg(riot_module_periph_gpio)]
pub mod gpio;

#[cfg(riot_module_bluetil_ad)]
pub mod bluetil;

pub mod nimble {
    #[cfg(riot_module_nimble_host)]
    pub mod uuid;
}

#[cfg(riot_module_ws281x)]
pub mod ws281x;

#[cfg(riot_module_microbit)]
pub mod microbit;

#[cfg(riot_module_vfs)]
pub mod vfs;

mod impl_critical_section;
pub mod interrupt;
#[path = "main_module.rs"]
pub mod main;

pub mod led;

#[cfg(riot_module_auto_init)]
pub mod auto_init;

// Gated like socket_embedded_nal_async_udp as it is only used there -- expand as needed.
#[cfg(all(
    riot_module_sock_udp,
    riot_module_sock_aux_local,
    feature = "with_embedded_nal_async"
))]
mod async_helpers;

#[cfg(riot_module_periph_uart)]
pub mod uart;
