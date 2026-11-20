//! A rewrite of the SPI wrappers that uses the [`embedded-hal` 1.0 model](embedded_hal::spi) of
//! SPI.
//!
//! This is a dedicated module because the abstractions of that version differ significantly from
//! what was in 0.2 that inspired the original implementation. With the next breaking release of
//! `riot-wrappers`, this module will replace the [`super`] module (possibly with a compatibility
//! alias).
//!
//! # HAL author notes
//!
//! The `embedded-hal` documentations has some [notes for HAL
//! authors](https://docs.rs/embedded-hal/latest/embedded_hal/spi/index.html#for-hal-authors); not
//! all those are followed here:
//!
//! * "HALS **must not** add infrastructure for sharing at the `SpiBus` level": RIOT already has
//!   that infrastructure; users don't have a guarantee on exclusive, but that's not even SPI
//!   specific: For all it's worth, the SPI hardware can be multiplexed into (say) I2C hardware,
//!   or anything else (hey maybe it's bit banging and the bitbanging core is just busy). At any
//!   rate, this crate is not going out of its way to do that, it's a consequence of being in the
//!   operating system, and technically it's not worse than being preempted by some interrupt.

use crate::error::{NegativeErrorExt, NumericError};
use core::convert::Infallible;
use core::num::NonZero;
use embedded_hal::spi::{ErrorType, Mode, Operation};

// The way that cpu/stm32/include/periph/cpu_spi.h defines its enum values through a macro causes
// them to be unprefixed for unknown reasons.
//
// This may become better when running a bindgen update, or when going with
// <https://github.com/RIOT-OS/rust-riot-sys/pull/32>.
#[cfg(accessible_riot_sys_spi_clk_t_SPI_CLK_100KHZ)]
mod frequency {
    pub(super) const SPI_CLK_100KHZ: riot_sys::spi_clk_t = riot_sys::spi_clk_t_SPI_CLK_100KHZ;
    pub(super) const SPI_CLK_400KHZ: riot_sys::spi_clk_t = riot_sys::spi_clk_t_SPI_CLK_400KHZ;
    pub(super) const SPI_CLK_1MHZ: riot_sys::spi_clk_t = riot_sys::spi_clk_t_SPI_CLK_1MHZ;
    pub(super) const SPI_CLK_5MHZ: riot_sys::spi_clk_t = riot_sys::spi_clk_t_SPI_CLK_5MHZ;
    pub(super) const SPI_CLK_10MHZ: riot_sys::spi_clk_t = riot_sys::spi_clk_t_SPI_CLK_10MHZ;
}
#[cfg(not(accessible_riot_sys_spi_clk_t_SPI_CLK_100KHZ))]
mod frequency {
    pub(super) const SPI_CLK_100KHZ: riot_sys::spi_clk_t = riot_sys::SPI_CLK_100KHZ;
    pub(super) const SPI_CLK_400KHZ: riot_sys::spi_clk_t = riot_sys::SPI_CLK_400KHZ;
    pub(super) const SPI_CLK_1MHZ: riot_sys::spi_clk_t = riot_sys::SPI_CLK_1MHZ;
    pub(super) const SPI_CLK_5MHZ: riot_sys::spi_clk_t = riot_sys::SPI_CLK_5MHZ;
    pub(super) const SPI_CLK_10MHZ: riot_sys::spi_clk_t = riot_sys::SPI_CLK_10MHZ;
}
use frequency::*;

/// A RIOT SPI device combined with complete with mode and clock configuration, but no particular
/// CS pin.
///
/// Note that while this implements [`embedded-hal::SpiBus`], it is not exclusive (because no
/// peripheral in RIOT is); when accessed while another "owner" uses it, operations only start when
/// the other party is done.
pub struct SpiBus {
    bus: riot_sys::spi_t,
    mode: riot_sys::spi_mode_t,
    clk: riot_sys::spi_clk_t,
}

/// A RIOT SPI device combined with its CS pin, complete with mode and clock configuration.
pub struct SpiDevice {
    bus: SpiBus,
    cs: riot_sys::spi_cs_t,
}

impl SpiBus {
    /// Creates a new SPI device, given its RIOT bus number (equivalent to running
    /// `SPI_DEV(number)`).
    ///
    /// By default, the clock speed is set to the lowest speed supported by the hardware, and the
    /// mode is set to SPI mode number 0 (the most common one).
    pub fn from_number(number: u32) -> Self {
        // SAFETY: This is designed to be called with any number. (Whether the device is then valid
        // will show later).
        let bus = unsafe { riot_sys::macro_SPI_DEV(number) };
        Self {
            bus,
            mode: riot_sys::spi_mode_t_SPI_MODE_0,
            clk: SPI_CLK_100KHZ,
        }
    }

    /// Uses the bus with a particular CS pin, creating an [`embedded_hal::SpiDevice`].
    ///
    /// Convenience alias for [`SpiDevice::new()`] for builder style construction.
    #[cfg(riot_module_periph_gpio)]
    pub fn into_device(self, cs: crate::gpio::GPIO) -> Result<SpiDevice, NumericError> {
        SpiDevice::new(self, cs)
    }

    // This family of speed setters is deliberately by-function, because this can easily be kept
    // available no matter how RIOT decides to support arbitrary speeds.

    /// Sets the speed to 100KHz.
    pub fn with_speed_100khz(self) -> Self {
        Self {
            clk: SPI_CLK_100KHZ,
            ..self
        }
    }

    /// Sets the speed to 400KHz.
    pub fn with_speed_400khz(self) -> Self {
        Self {
            clk: SPI_CLK_400KHZ,
            ..self
        }
    }

    /// Sets the speed to 1MHz.
    pub fn with_speed_1mhz(self) -> Self {
        Self {
            clk: SPI_CLK_1MHZ,
            ..self
        }
    }

    /// Sets the speed to 5MHz.
    pub fn with_speed_5mhz(self) -> Self {
        Self {
            clk: SPI_CLK_5MHZ,
            ..self
        }
    }

    /// Sets the speed to 10MHz.
    pub fn with_speed_10mhz(self) -> Self {
        Self {
            clk: SPI_CLK_10MHZ,
            ..self
        }
    }

    /// Sets the device's mode.
    pub fn with_mode(self, mode: Mode) -> Self {
        Self {
            mode: match mode {
                embedded_hal::spi::MODE_0 => riot_sys::spi_mode_t_SPI_MODE_0,
                embedded_hal::spi::MODE_1 => riot_sys::spi_mode_t_SPI_MODE_1,
                embedded_hal::spi::MODE_2 => riot_sys::spi_mode_t_SPI_MODE_2,
                embedded_hal::spi::MODE_3 => riot_sys::spi_mode_t_SPI_MODE_3,
            },
            ..self
        }
    }
}

impl ErrorType for SpiBus {
    type Error = Infallible;
}

// In RIOT, there is no type-level distinction between a bus and a device -- if it's acquired with
// GPIO_UNDEF as the CS pin, it matches the SpiBus pattern, and if it has a pin in it, it matches
// the SpiDevice pattern. (The distinction of whether something is owned exclusively or not can not
// be made in RIOT as it has no concept of exclusive device ownership.)
//
// To avoid re-implementing everything and especially the split transfer logic, this is hooking
// into the transaction function, which is largely modeled after the HAL SpiDevice's API (taking an
// Operations list like SpiDevice::transaction()).
//
// This may or may not be efficient depending on what the compiler inlines, but really, if you want
// efficient, go with SpiDevice anyway for hardware CS. Another downside of this approach is that
// it means that ZTimer is a dependency even when not using SpiDevice; sticking with it for overall
// simplicity.
impl embedded_hal::spi::SpiBus for SpiBus {
    fn read(&mut self, words: &mut [u8]) -> Result<(), Infallible> {
        transaction(
            &self,
            riot_sys::inline::GPIO_UNDEF.try_into().unwrap(),
            &mut [Operation::Read(words)],
        );
        Ok(())
    }
    fn write(&mut self, words: &[u8]) -> Result<(), <Self as embedded_hal::spi::ErrorType>::Error> {
        transaction(
            &self,
            riot_sys::inline::GPIO_UNDEF.try_into().unwrap(),
            &mut [Operation::Write(words)],
        );
        Ok(())
    }
    fn transfer(&mut self, read: &mut [u8], write: &[u8]) -> Result<(), Infallible> {
        transaction(
            &self,
            riot_sys::inline::GPIO_UNDEF.try_into().unwrap(),
            &mut [Operation::Transfer(read, write)],
        );
        Ok(())
    }
    fn transfer_in_place(&mut self, words: &mut [u8]) -> Result<(), Infallible> {
        transaction(
            &self,
            riot_sys::inline::GPIO_UNDEF.try_into().unwrap(),
            &mut [Operation::TransferInPlace(words)],
        );
        Ok(())
    }
    fn flush(&mut self) -> Result<(), Infallible> {
        // See also comment on `SpiDevice for SpiDevice`
        Ok(())
    }
}

impl SpiDevice {
    /// and its CS GPIO pin
    #[cfg(riot_module_periph_gpio)]
    pub fn new(bus: SpiBus, cs: crate::gpio::GPIO) -> Result<Self, NumericError> {
        // spi_cs_t can be many things, but as the `spi_init_cs` documentation says that one can
        // put a GPIO_PIN value in there, let's hope that this is only used where integer promotion
        // actually makes sense (and where it makes sense, there is a .into()).
        let cs: riot_sys::spi_cs_t = cs.to_c().into();
        (unsafe { riot_sys::spi_init_cs(bus.bus, cs) }).negative_to_error()?;
        Ok(Self { bus, cs })
    }
}

impl ErrorType for SpiDevice {
    type Error = Infallible;
}

impl embedded_hal::spi::SpiDevice for SpiDevice {
    // No need to implement flush(): It's not very explicit, but as spi_release() docs say that
    // after release, the SPI bus should be powered down, that only works if release blocks until
    // that is done.

    fn transaction(&mut self, ops: &mut [Operation<'_, u8>]) -> Result<(), Infallible> {
        transaction(&self.bus, self.cs, ops);
        Ok(())
    }
}

fn transaction(bus: &SpiBus, cs: riot_sys::spi_cs_t, ops: &mut [Operation<'_, u8>]) {
    unsafe { riot_sys::spi_acquire(bus.bus, cs, bus.mode, bus.clk) };
    let len = ops.len();
    for (index, op) in ops.iter_mut().enumerate() {
        let cont = index != len - 1;
        match op {
            Operation::Read(bytes) => unsafe {
                riot_sys::spi_transfer_bytes(
                    bus.bus,
                    cs,
                    cont,
                    core::ptr::null(),
                    bytes.as_mut_ptr() as _,
                    bytes.len().try_into().expect("usize and size_t match"),
                );
            },
            Operation::Write(bytes) => unsafe {
                riot_sys::spi_transfer_bytes(
                    bus.bus,
                    cs,
                    cont,
                    bytes.as_ptr() as _,
                    core::ptr::null_mut(),
                    bytes.len().try_into().expect("usize and size_t match"),
                );
            },
            Operation::Transfer(read, write) => unsafe {
                let common_length = core::cmp::min(read.len(), write.len());
                let read_longer = read
                    .len()
                    .checked_sub(write.len())
                    .and_then(|n| NonZero::try_from(n).ok());
                let write_longer = write
                    .len()
                    .checked_sub(read.len())
                    .and_then(|n| NonZero::try_from(n).ok());
                riot_sys::spi_transfer_bytes(
                    bus.bus,
                    cs,
                    cont || read_longer.is_some() || write_longer.is_some(),
                    write.as_ptr() as _,
                    read.as_mut_ptr() as _,
                    common_length.try_into().expect("usize and size_t match"),
                );
                if let Some(read_longer) = read_longer {
                    riot_sys::spi_transfer_bytes(
                        bus.bus,
                        cs,
                        cont,
                        core::ptr::null(),
                        read[common_length..].as_mut_ptr() as _,
                        read_longer
                            .get()
                            .try_into()
                            .expect("usize and size_t match"),
                    );
                }
                if let Some(write_longer) = write_longer {
                    riot_sys::spi_transfer_bytes(
                        bus.bus,
                        cs,
                        cont,
                        write[common_length..].as_ptr() as _,
                        core::ptr::null_mut(),
                        write_longer
                            .get()
                            .try_into()
                            .expect("usize and size_t match"),
                    );
                }
            },
            Operation::TransferInPlace(bytes) => unsafe {
                riot_sys::spi_transfer_bytes(
                    bus.bus,
                    cs,
                    cont,
                    bytes.as_ptr() as _,
                    bytes.as_mut_ptr() as _,
                    bytes.len().try_into().expect("usize and size_t match"),
                );
            },
            Operation::DelayNs(time) => {
                crate::ztimer::Clock::usec().sleep(crate::ztimer::Ticks(time.div_ceil(1000)));
            }
        }
    }
    // SAFETY: as per C API.
    unsafe { riot_sys::spi_release(bus.bus) };
}
