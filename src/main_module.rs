//! Tools for providing a RIOT main function
//!
//! The main contribution of this module is the [`riot_main!`](super::riot_main!) macro.
//!
//! The alternative to using that (other than doing it manually) is to have C code along with the
//! Rust application that occupies the main function.
//!
//! In these cases, Rust code can be called into from the main C code by declaring the entry
//! functions `#[no_mangle] pub extern "C"`, and having analogous `extern` functions in the calling
//! C code.

use crate::stdio::println;
use crate::thread::{EndToken, StartToken};

// General alternative to this module: Build the extern "C" main all the time and request that the
// application implement a named function. I never got the main function to be carried to the
// linker step, though. If implemented like this, the module needs to be gated like
// set_panic_handler.
//
// extern "Rust" {
//     fn riot_main();
// }
//
// #[no_mangle]
// pub extern "C" fn main() -> u32 {
//     unsafe { riot_main() };
//     0
// }

use core::fmt;

mod sealed {
    pub trait Sealed<Variant> {}
}

use sealed::Sealed;

// The Variant argument is really just taking different types to allow "conflicting"
// implementations that are not conflicting but just ambiguous as long as nobody forces the Variant
// argument. Conveniently, that ambiguity is accepted.
//
// Thanks to Charles from #rust:matrix.org for pointing out this neat trick.
#[doc(hidden)]
pub trait UsableAsMain<Variant>: Sealed<Variant> {
    unsafe fn call_main(&self) -> i32;
}

// Beware that the following are *not* checked for being conflicting (because they are not), but if
// there were any situation of ambiguity, the main macro would break.

impl<F: Fn() -> T, T: Termination> Sealed<[u8; 1]> for F {}

impl<F: Fn() -> T, T: Termination> UsableAsMain<[u8; 1]> for F {
    unsafe fn call_main(&self) -> i32 {
        (self)().report()
    }
}

impl<F: Fn(StartToken) -> crate::never::Never> Sealed<[u8; 2]> for F {}

impl<F: Fn(StartToken) -> crate::never::Never> UsableAsMain<[u8; 2]> for F {
    unsafe fn call_main(&self) -> i32 {
        // unsafe: By construction of the C main function this only happens at startup time
        // with a thread that hasn't done anything relevant before.
        let unique = crate::thread::StartToken::new();

        (self)(unique)
    }
}

impl<F: Fn(StartToken) -> ((), EndToken)> Sealed<[u8; 3]> for F {}

impl<F: Fn(StartToken) -> ((), EndToken)> UsableAsMain<[u8; 3]> for F {
    unsafe fn call_main(&self) -> i32 {
        // unsafe: By construction of the C main function this only happens at startup time
        // with a thread that hasn't done anything relevant before.
        let unique = crate::thread::StartToken::new();

        // We're not really consuming the token, just require that the function can provide it and
        // doesn't just return without having invalidated all users of its PID
        let (termination, _token) = (self)(unique);
        termination.report()
    }
}

/// To have a nice Rust main function, run the `riot_main!` macro with the name of your main
/// function an item (ie. top level in a module) in your crate. The function identified by it must
/// return something that implements the Termination trait.
///
/// Example:
///
/// ```
/// # #![no_std]
/// # #![no_main]
/// use riot_wrappers::riot_main;
/// riot_main!(main);
///
/// fn main() {
///     unimplemented!()
/// }
/// ```
///
/// Functions with multiple signatures are accepted:
///
/// * `fn main()` -- useful for very simple programs
/// * `fn main() -> impl Termination` -- prints the error message according to the [Termination]
///   implementation (in particular, [Result] types with a [Debug] error are useful here)
/// * `fn main(tokens: StartToken) -> (impl Termination, EndToken)` -- this ensures that
///   the program has full control over the main thread. As a [StartToken] allows doing things that
///   require undoing before the thread may terminate (eg. subscribing it to messages), an
///   [EndToken] needs to be produced before the thread can terminate with a message as
///   above.
/// * `fn main(tokens: StartToken) -> !` -- a frequently useful variation thereof for main loops
///   that are loops anyway.
#[macro_export]
macro_rules! riot_main {
    ($main:ident) => {
        #[export_name = "main"]
        pub extern "C" fn c_main() -> i32 {
            unsafe { <_ as $crate::main::UsableAsMain<_>>::call_main(&$main) }
        }
    };
}

/// A result trait for main methods, analogous to std::process::Termination
pub trait Termination {
    fn report(self) -> i32;
}

impl Termination for () {
    fn report(self) -> i32 {
        0
    }
}

impl Termination for i32 {
    fn report(self) -> i32 {
        self
    }
}

// Copied and stripped down from std

impl<E: fmt::Debug> Termination for Result<(), E> {
    fn report(self) -> i32 {
        match self {
            Ok(()) => ().report(),
            Err(err) => Err::<crate::never::Never, _>(err).report(),
        }
    }
}

#[cfg(not(rustc_has_never))]
impl Termination for crate::never::Never {
    fn report(self) -> i32 {
        self
    }
}

impl Termination for core::convert::Infallible {
    fn report(self) -> i32 {
        match self {}
    }
}

impl<E: fmt::Debug> Termination for Result<crate::never::Never, E> {
    fn report(self) -> i32 {
        match self {
            Err(err) => {
                println!("Error: {:?}", err);
                1
            }
            #[allow(unreachable_patterns)] // reason: RIOT CI's stable is still 1.77
            Ok(never) => never,
        }
    }
}
