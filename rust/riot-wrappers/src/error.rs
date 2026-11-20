//! Common error handling components for the RIOT operating system
//!
//! Most fallible operations in the wrappers produce a [NumericError], which is a slightly more
//! precise wrapper around a negative integer. The [NegativeErrorExt::negative_to_error()] trait
//! method can be used to produce such errors when creating wrappers around C functions.
//!
//! ## Constants
//!
//! Several commonly used errors are provided as constants rather than requiring the use of
//! [NumericError::from_constant] for easier use. That list is not created comprehensively but
//! populated on demand. (Copying the full list would needlessly limit RIOT's ability to slim down
//! the list).

use core::convert::TryInto;
use core::ffi::CStr;
use core::num::NonZero;

pub trait NegativeErrorExt {
    type Out;

    /// Convert to a Result that is successful if the input value is zero or positive, or a
    /// NumericError if it is negative
    fn negative_to_error(self) -> Result<Self::Out, NumericError>;
}

/// An error that is expressed as a negative number
///
/// Ideally, that constraint should be expressed in the type system to allow the compiler to
/// represent `Result<positive_usize, NumericError>` as just the isize it originally was. For the
/// time being, this works well enough, and performance evaluation can later be done against a
/// manually implemented newtype around isize that'd be used to represent the Result.
#[derive(Debug, PartialEq, Eq)]
pub struct NumericError {
    // The NonZero doesn't cover the full desired range, but at least Result<(), NumericError> can
    // be lean.
    number: NonZero<isize>,
}

impl NumericError {
    /// Construct a NumericError from a [riot_sys] constant
    ///
    /// As error constants are in their unsigned positive form, this flips the argument's sign into
    /// the negative range.
    ///
    /// ```
    /// # #![no_std]
    /// # #![feature(start)]
    /// # #[start]
    /// # fn main(_argc: isize, _argv: *const *const u8) -> isize {
    /// # use riot_wrappers::error::NumericError;
    /// # use riot_wrappers::stdio::println;
    /// let err = NumericError::from_constant(riot_sys::ENOTSUP as _);
    /// println!("{:?}", err); // NumericError { number: -61 }
    /// # 0
    /// # }
    /// ```
    ///
    /// ## Panics
    ///
    /// In debug mode, this ensures that the given error is greater than zero.
    pub const fn from_constant(name: isize) -> Self {
        debug_assert!(
            name > 0,
            "Error names are expected to be positive for conversion into negative error numbers."
        );
        // Can be an `.unwrap()` once feature(const_trait_impl) is stabilized
        let number = match NonZero::new(name) {
            Some(n) => n,
            _ => panic!("Error names are expected to be positive for conversion into negative error numbers.")
        };
        NumericError {
            number: number.saturating_neg(),
        }
    }

    /// Numeric value of the error
    pub const fn number(&self) -> isize {
        self.number.get()
    }

    /// Convert the error into an [nb::Error] that is [nb::Error::WouldBlock] if the error is
    /// `-EAGAIN`, and an actual error otherwise.
    pub fn again_is_wouldblock(self) -> nb::Error<Self> {
        if self == EAGAIN {
            return nb::Error::WouldBlock;
        }
        nb::Error::Other(self)
    }

    fn string(&self) -> Option<&'static CStr> {
        #[cfg(all(riot_module_tiny_strerror, not(riot_module_tiny_strerror_minimal)))]
        // unsafe: According to C API
        // number cast: Disagreements on the numeric error size
        // string cast: Disagreements on the signedness of char
        return Some(unsafe {
            CStr::from_ptr(riot_sys::tiny_strerror(-self.number.get() as _) as _)
        });

        #[cfg(not(all(riot_module_tiny_strerror, not(riot_module_tiny_strerror_minimal))))]
        return None;
    }
}

impl core::fmt::Display for NumericError {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> Result<(), core::fmt::Error> {
        if let Some(s) = self.string() {
            write!(f, "Error {} ({})", self.number(), s.to_str().unwrap())?;
        } else {
            write!(f, "Error {}", self.number())?;
        }
        Ok(())
    }
}

impl core::error::Error for NumericError {}

impl<T> NegativeErrorExt for T
where
    T: num_traits::Zero + core::cmp::PartialOrd + TryInto<isize>,
{
    type Out = T;

    fn negative_to_error(self) -> Result<Self::Out, NumericError> {
        if self >= Self::zero() {
            Ok(self)
        } else {
            Err(self
                .try_into()
                .ok()
                .and_then(NonZero::new)
                .map(|number| NumericError { number })
                .unwrap_or(EOVERFLOW))
        }
    }
}

macro_rules! E {
    ($e:ident) => {
        #[doc = concat!("The predefined error ", stringify!($e))]
        pub const $e: NumericError = NumericError::from_constant(riot_sys::$e as _);
    };
}

// See module level comment
E!(EAGAIN);
E!(EINVAL);
E!(ENODEV);
E!(ENOMEM);
E!(ENOSPC);
E!(ENOTCONN);
E!(ENOTSUP);
E!(EOVERFLOW);
