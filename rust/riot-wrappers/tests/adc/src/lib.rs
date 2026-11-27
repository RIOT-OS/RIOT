#![no_std]

use riot_wrappers::adc;
use riot_wrappers::println;
use riot_wrappers::riot_main;

riot_main!(main);

fn main() {
    use embedded_hal::adc::OneShot;

    let mut adc = adc::ADC {
        resolution: riot_sys::adc_res_t_ADC_RES_8BIT,
    };
    let mut line = unsafe { adc::ADCLine::init(0) }.unwrap();
    loop {
        let value = adc.read(&mut line).unwrap();
        println!("ADC 0 Value: {:?}", value);
    }
}
