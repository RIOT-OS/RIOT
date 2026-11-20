#![no_std]

use riot_wrappers::dac;
use riot_wrappers::riot_main;

riot_main!(main);

fn main() {
    let mut dac = dac::DACLine::new(0).unwrap();
    dac.set(655); // 1% of maximum value
}
