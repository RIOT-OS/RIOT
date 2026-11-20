//! This is a primitive I2C scanner, and should thus report *something* interesting if any I2C
//! device is connected. (And reading should be safe on any device / bus).
#![no_std]

use embedded_hal::i2c::I2c;

use riot_wrappers::println;
use riot_wrappers::riot_main;

riot_main!(main);

fn main() {
    let mut i2c = riot_wrappers::i2c::I2CDevice::new(0); // FIXME from_number?

    let mut buf = [0];

    loop {
        for i in 0..=127 {
            match i2c.read(i, &mut buf) {
                Ok(()) => println!("From {i}, read bytes: {:?}", &buf),
                Err(e) => println!("From {i}, error {e:?}"),
            }
        }
    }
}
