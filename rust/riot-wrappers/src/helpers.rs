//! Small tools used in different wrappers without being actually public

/// Generalization of the Never type extracting workaround from
/// <https://github.com/rust-lang/rust/issues/43301#issuecomment-912390203> -- also useful to
/// extract return types of functions that (in what is compatible behavior in C) change their
/// return types.
///
/// With the introduction of ArgXType, it's actually not a ReturnType but AnyFunctionInvolvedType
/// extractor any more...
pub trait ReturnTypeExtractor {
    type ReturnType;
    type Arg1Type;
    type Arg2Type;
}
impl<T> ReturnTypeExtractor for fn() -> T {
    type ReturnType = T;
    type Arg1Type = ();
    type Arg2Type = ();
}
impl<T, I1> ReturnTypeExtractor for Option<unsafe extern "C" fn(I1) -> T> {
    type ReturnType = T;
    type Arg1Type = I1;
    type Arg2Type = ();
}
impl<T, I1, I2> ReturnTypeExtractor for Option<unsafe extern "C" fn(I1, I2) -> T> {
    type ReturnType = T;
    type Arg1Type = I1;
    type Arg2Type = I2;
}

/// Trait that eases conversions from a char pointer (no matter the signedness, they are used
/// inconsistently in RIOT) to a CStr. The result is often used with `?.to_str().ok()?`.
pub(crate) trait PointerToCStr {
    /// Cast self around until it is suitable input to [`core::ffi::CStr::from_ptr()`], and run
    /// that function. See there for safety requirements; in particular, the user needs to ensure
    /// that the lifetime is suitable.
    ///
    /// This returns None if self is the null pointer.
    unsafe fn to_lifetimed_cstr<'a>(self) -> Option<&'a core::ffi::CStr>;
}

// Depending on the platform's default signedness of char, one of the casts is unnecessary.

impl PointerToCStr for *const u8 {
    unsafe fn to_lifetimed_cstr<'a>(self) -> Option<&'a core::ffi::CStr> {
        if self == core::ptr::null() {
            None
        } else {
            Some(core::ffi::CStr::from_ptr(self as *const core::ffi::c_char))
        }
    }
}

impl PointerToCStr for *const i8 {
    unsafe fn to_lifetimed_cstr<'a>(self) -> Option<&'a core::ffi::CStr> {
        if self == core::ptr::null() {
            None
        } else {
            Some(core::ffi::CStr::from_ptr(self as *const core::ffi::c_char))
        }
    }
}

// Gated because it is only used there -- expand as needed.
#[cfg(riot_module_vfs)]
mod slice_to_cstr {
    /// Trait that eases conversions from a char slice (no matter the signedness, they are used
    /// inconsistently in RIOT) to a CStr. The result is often used with `?.to_str().ok()?`.
    pub(crate) trait SliceToCStr {
        /// Cast self around until it is suitable input to [`core::ffi::CStr::from_bytes_until_nul()`],
        /// and run that function.
        ///
        /// Note that while "the slice until any null byte" could be safely used in Rust (as a slice or
        /// even a str), its presence in C practically always indicates an error, also because that
        /// data wouldn't be usable by other C code using its string conventions.
        ///
        /// It is using a local error type because while the semantics of `from_bytes_until_nul` are
        /// the right ones considering how this is used on C fields that are treated with `strlen()`
        /// etc., that function is not stable yet and emulated.
        fn to_cstr(&self) -> Result<&core::ffi::CStr, FromBytesUntilNulError>;
    }

    // Unlike in the from_ptr case, this is consistently taking u8, so only the i8 case gets casting.

    impl SliceToCStr for [u8] {
        fn to_cstr(&self) -> Result<&core::ffi::CStr, FromBytesUntilNulError> {
            // Emulate from_bytes_until_null
            let index = self
                .iter()
                .position(|&c| c == 0)
                .ok_or(FromBytesUntilNulError {})?;

            core::ffi::CStr::from_bytes_with_nul(&self[..index + 1])
                // Actually the error is unreachable
                .map_err(|_| FromBytesUntilNulError {})
        }
    }

    impl SliceToCStr for [i8] {
        fn to_cstr(&self) -> Result<&core::ffi::CStr, FromBytesUntilNulError> {
            let s: &[u8] = unsafe { core::mem::transmute(self) };
            s.to_cstr()
        }
    }

    /// Error from [SliceToCStr::to_cstr].
    ///
    /// This will become [core::ffi::c_str::FromBytesUntilNulError] once that's stable, and may be changed
    /// without a breaking release even though it's technically a breaking change. (At this point, that
    /// type will be `pub use`d here and deprecated).
    #[derive(Debug)]
    pub struct FromBytesUntilNulError {}
}
#[cfg(riot_module_vfs)]
pub use slice_to_cstr::*;
