//! Controlling the I²C bus

pub mod impl_1;

use riot_sys::i2c_t;

/// An I²C master backed by RIOT's [I2C implementation]
///
/// [I2C implementation]: http://doc.riot-os.org/group__drivers__periph__i2c.html
///
/// Actual transactions on this are performed through the [embedded_hal_0_2::blocking::i2c] traits
/// implemented by this.
#[derive(Debug)]
pub struct I2CDevice {
    dev: i2c_t,
}

impl I2CDevice {
    /// Create a new I2CDevice from a RIOT descriptor
    ///
    /// As all transactions on the bus are gated by acquire / release steps implied in the
    /// individual reads or writes, multiple copies of the same device can safely coexist.
    pub fn new(dev: i2c_t) -> Self {
        I2CDevice { dev }
    }
}
