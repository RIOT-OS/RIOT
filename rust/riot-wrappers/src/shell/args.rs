use crate::libc;

use crate::helpers::PointerToCStr;

/// Newtype around an (argc, argv) C style string array that presents itself as much as an `&'a
/// [&'a str]` as possible. (Slicing is not implemented for reasons of laziness).
///
/// As this is used with the command line parser, it presents the individual strings as &str
/// infallibly. If non-UTF8 input is received, a variation of from_utf8_lossy is applied: The
/// complete string (rather than just the bad characters) is reported as "�", but should have the
/// same effect: Be visible as an encoding error without needlessly complicated error handling for
/// niche cases.
pub struct Args<'a>(&'a [*mut libc::c_char]);

unsafe fn argconvert<'a>(data: *mut libc::c_char) -> &'a str {
    data.to_lifetimed_cstr()
        .expect("Command-line arguments are non-null")
        .to_str()
        .unwrap_or("�")
}

impl<'a> Args<'a> {
    /// Create the slice from its parts.
    ///
    /// ## Unsafe
    ///
    /// argv must be a valid pointer, and its first argc items must be valid pointers. The
    /// underlying char strings do not need to be valid UTF-8, but must be null terminated.
    pub unsafe fn new(
        argc: libc::c_int,
        argv: *const *const libc::c_char,
        _lifetime_marker: &'a (),
    ) -> Self {
        Args(core::slice::from_raw_parts(argv as _, argc as usize))
    }

    /// Returns an iterator over the arguments.
    pub fn iter(&self) -> ArgsIterator<'a> {
        ArgsIterator(self.0.iter())
    }

    /// Returns the argument in the given position.
    pub fn get(&self, index: usize) -> Option<&str> {
        if index < self.0.len() {
            Some(unsafe { argconvert(self.0[index]) })
        } else {
            None
        }
    }

    /// Length of the arguments list
    pub fn len(&self) -> usize {
        self.0.len()
    }
}

/// Iterator of [Args], created using [Args::iter()]
pub struct ArgsIterator<'a>(core::slice::Iter<'a, *mut libc::c_char>);

impl<'a> core::iter::Iterator for ArgsIterator<'a> {
    type Item = &'a str;

    fn next(&mut self) -> Option<Self::Item> {
        let data = self.0.next()?;

        Some(unsafe { argconvert(*data) })
    }
}

impl<'a> ExactSizeIterator for ArgsIterator<'a> {}

impl<'a> IntoIterator for Args<'a> {
    type Item = &'a str;
    type IntoIter = ArgsIterator<'a>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

impl<'a> core::ops::Index<usize> for Args<'a> {
    type Output = str;

    fn index(&self, i: usize) -> &str {
        unsafe { argconvert(self.0[i]) }
    }
}
