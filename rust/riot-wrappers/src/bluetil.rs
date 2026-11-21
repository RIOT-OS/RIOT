//! [Bluetil](https://doc.riot-os.org/group__ble__bluetil__ad.html) tools for BLE Advertising Data (AD)

use crate::error::NegativeErrorExt;
use core::convert::TryInto;
use riot_sys::bluetil_ad_t;

/// Wrapper around [bluetil_ad](https://doc.riot-os.org/group__ble__bluetil__ad.html) (BLE Advertising Data)
///
/// This is implemented as a possibly owned buffer, as that should make usage straightforward both
/// for read-only data (not practically usable yet) and for writing; it remains to be seen whether
/// that's viable.
pub struct Ad<B: AsRef<[u8]>>(B);

impl<B: AsRef<[u8]>> Ad<B> {
    /// Reclaim the buffer that the AD was built around.
    ///
    /// This is particularly useful when an AD object was populated, and in the end the buffer
    /// needs to be obtained in order to send it.
    pub fn destroy(self) -> B {
        self.0
    }
}

// FIXME: Test this with actual read-only data

#[derive(Debug)]
pub enum Error {
    NotFound,
    NoMem,
}

impl From<crate::error::NumericError> for Error {
    fn from(e: crate::error::NumericError) -> Error {
        match e.number() as _ {
            riot_sys::BLUETIL_AD_NOTFOUND => Error::NotFound,
            riot_sys::BLUETIL_AD_NOMEM => Error::NoMem,
            _ => panic!("Invalid bluetil error"),
        }
    }

    // FIXME: Add all the find functions
}

// FIXME: flags and type are u32 because the riot_sys constants are; wrap?

impl<const L: usize> Ad<heapless::Vec<u8, L>> {
    /// Create an empty AD object around owned memory; the size is given by the type parameter.
    pub fn new() -> Self {
        Self(heapless::Vec::new())
    }

    /// Construct a bluetil_ad_t that represent the current vec state
    ///
    /// This is not unsafe in itself, but usually used with functions that are, and when they
    /// write into the buffer, it needs the unsafe [heapless::Vec::set_len] to propagate that write.
    fn build(&self) -> bluetil_ad_t {
        bluetil_ad_t {
            buf: self.0.as_ptr() as _,
            pos: self.0.len() as _,
            // As this is checked here, all the other casts of pos are OK too
            size: self.0.capacity().try_into().unwrap(),
        }
    }

    /// Add a "flags" field with the given flag value to the AD (convenience function)
    #[doc(alias = "bluetil_ad_add_flags")]
    pub fn add_flags(&mut self, flags: u32) -> Result<(), Error> {
        let mut ad = self.build();
        // unsafe: regular C call
        unsafe {
            riot_sys::bluetil_ad_add_flags(crate::inline_cast_mut(&mut ad as *mut _), flags as _)
        }
        .negative_to_error()?;
        // unsafe: bluetil doesn't set pos after size
        unsafe { self.0.set_len(ad.pos as _) };
        Ok(())
    }

    /// Add an arbitrary typed field to the AD
    #[doc(alias = "bluetil_ad_add")]
    pub fn add(&mut self, type_: u32, data: &[u8]) -> Result<(), Error> {
        let mut ad = self.build();
        // unsafe: regular C call
        unsafe {
            riot_sys::bluetil_ad_add(&mut ad, type_ as _, data.as_ptr() as _, data.len() as _)
        }
        .negative_to_error()?;
        // unsafe: bluetil doesn't set pos after size
        unsafe { self.0.set_len(ad.pos as _) };
        Ok(())
    }
}

// FIXME: 31 is expanded from BLE_HCI_MAX_ADV_DATA_LEN
impl Ad<heapless::Vec<u8, 31>> {
    /// Create an empty AD object around owned memory with the maximum sendable size.
    ///
    /// This is often more convenient than `new` as it's not too large anyway (31 bytes).
    pub fn new_maximal() -> Self {
        Self(heapless::Vec::new())
    }
}
