#[cfg(riot_module_ztimer_usec)]
pub mod for_embedded_hal_1;

use core::convert::Infallible;
use embedded_hal_0_2::blocking;
use riot_sys::{
    spi_acquire, spi_clk_t, spi_cs_t, spi_mode_t, spi_release, spi_t, spi_transfer_bytes,
};

pub struct SPIDevice(spi_t);

pub struct AcquiredSPI<'a> {
    device: &'a mut SPIDevice,
    cs: spi_cs_t,
}

impl<'a> Drop for AcquiredSPI<'a> {
    fn drop(&mut self) {
        unsafe { spi_release(self.device.0) };
    }
}

impl SPIDevice {
    /// Create an SPI device from an `spi_t`
    pub fn from_c(bus: spi_t) -> Self {
        Self(bus)
    }

    /// Create an SPI device from the number it is assigned on the board
    pub fn from_number(bus: u32) -> Self {
        let bus = unsafe { riot_sys::macro_SPI_DEV(bus) };
        Self::from_c(bus)
    }

    pub fn acquire<'a>(
        &'a mut self,
        cs: spi_cs_t,
        mode: spi_mode_t,
        clk: spi_clk_t,
    ) -> AcquiredSPI<'a> {
        unsafe { spi_acquire(self.0, cs, mode, clk) };

        AcquiredSPI {
            device: self,
            cs: cs,
        }
    }
}

// Abandoning this for now as the lifetimes get tricky and this is all only a first sketch anyway
// trait AsAcquired<'a> {
//     fn as_acquired<'b: 'a>(&'b mut self) -> &'b mut AcquiredSPI<'a>;
// }
//
// // There was an attempt -- but it'd need defaults. Still, it might be nice to have something like
// // this from a struct around an SPIDevice and some defaults.
// //
// // impl<'a> AsAcquired<'a> for SPIDevice {
// //     fn as_acquired<'b: 'a>(&'b mut self) -> &'b mut AcquiredSPI<'a> {
// //         self.acquire()
// //     }
// // }
//
// impl<'a> AsAcquired<'a> for AcquiredSPI<'a> {
//     fn as_acquired<'b: 'a>(&'b mut self) -> &'b mut AcquiredSPI<'a> {
//         &mut self
//     }
// }

impl<'a> blocking::spi::Transfer<u8> for AcquiredSPI<'a> {
    type Error = Infallible;

    fn transfer<'w>(&mut self, words: &'w mut [u8]) -> Result<&'w [u8], Self::Error> {
        unsafe {
            spi_transfer_bytes(
                self.device.0,
                self.cs,
                false,
                words.as_ptr() as *const _,
                words.as_ptr() as *mut _,
                words.len() as _,
            )
        };
        Ok(words)
    }
}
