use embedded_hal::i2c;

use riot_sys::{i2c_acquire, i2c_read_bytes, i2c_release, i2c_write_bytes};

use crate::error::{NegativeErrorExt, NumericError};

use super::*;

const I2C_NOSTOP: u8 = riot_sys::i2c_flags_t_I2C_NOSTOP as _;
const I2C_NOSTART: u8 = riot_sys::i2c_flags_t_I2C_NOSTART as _;

#[derive(Debug)]
pub struct Error(NumericError);

impl From<NumericError> for Error {
    fn from(err: NumericError) -> Self {
        Self(err)
    }
}

impl i2c::Error for Error {
    fn kind(&self) -> i2c::ErrorKind {
        match -self.0.number() as _ {
            // The list documented with all the RIOT I2C functions
            riot_sys::EIO => i2c::ErrorKind::NoAcknowledge(i2c::NoAcknowledgeSource::Data),
            riot_sys::ENXIO => i2c::ErrorKind::NoAcknowledge(i2c::NoAcknowledgeSource::Address),
            riot_sys::ETIMEDOUT => i2c::ErrorKind::Other,
            riot_sys::EINVAL => i2c::ErrorKind::Other,
            riot_sys::EOPNOTSUPP => i2c::ErrorKind::Other, // We should avoid this at type level,
            // but can't because RIOT is not telling
            // us at setup time.
            riot_sys::EAGAIN => i2c::ErrorKind::ArbitrationLoss,
            _ => i2c::ErrorKind::Other,
        }
    }
}

impl i2c::ErrorType for I2CDevice {
    type Error = Error;
}

fn with_acquire<R>(dev: &mut I2CDevice, f: impl FnOnce(&mut I2CDevice) -> R) -> R {
    unsafe { i2c_acquire(dev.dev) };
    let result = f(dev);
    unsafe { i2c_release(dev.dev) };
    result
}

impl i2c::I2c<i2c::SevenBitAddress> for I2CDevice {
    fn transaction(
        &mut self,
        address: i2c::SevenBitAddress,
        operations: &mut [i2c::Operation<'_>],
    ) -> Result<(), Self::Error> {
        with_acquire(self, |dev| {
            #[derive(PartialEq)]
            enum LastOperation {
                Initial,
                Read,
                Write,
            }
            use LastOperation::*;

            impl LastOperation {
                fn from(op: &i2c::Operation) -> Self {
                    match op {
                        i2c::Operation::Read(_) => Read,
                        i2c::Operation::Write(_) => Write,
                    }
                }
            }

            let mut last_operation = Initial;
            let last_index = operations.len() - 1;

            for (i, op) in operations.iter_mut().enumerate() {
                let this_operation = LastOperation::from(op);
                let is_last = i == last_index;

                let maybe_nostop = if is_last { 0 } else { I2C_NOSTOP };

                // The regular read and write functions in RIOT automatically issue a repeated
                // start condition when called after a I2C_NOSTOP operation.
                if last_operation != this_operation {
                    match op {
                        i2c::Operation::Read(slice) => {
                            slice[0] = 0xff;
                            let result = (unsafe {
                                i2c_read_bytes(
                                    dev.dev,
                                    address as u16,
                                    slice.as_mut_ptr() as _,
                                    slice.len() as _,
                                    maybe_nostop,
                                )
                            })
                            .negative_to_error()?;
                            result
                        }
                        i2c::Operation::Write(slice) => (unsafe {
                            i2c_write_bytes(
                                dev.dev,
                                address as u16,
                                slice.as_ptr() as _,
                                slice.len() as _,
                                maybe_nostop,
                            )
                        })
                        .negative_to_error()?,
                    };
                } else {
                    // No "repeated start", no address, just a different scatter-gather slice
                    match op {
                        i2c::Operation::Read(slice) => (unsafe {
                            slice[0] = 0xff;
                            i2c_read_bytes(
                                dev.dev,
                                0,
                                slice.as_mut_ptr() as _,
                                slice.len() as _,
                                I2C_NOSTART | maybe_nostop,
                            )
                        })
                        .negative_to_error()?,
                        i2c::Operation::Write(slice) => (unsafe {
                            i2c_write_bytes(
                                dev.dev,
                                0,
                                slice.as_ptr() as _,
                                slice.len() as _,
                                I2C_NOSTART | maybe_nostop,
                            )
                        })
                        .negative_to_error()?,
                    };
                }

                last_operation = this_operation;
            }
            Ok(())
        })
    }
}
