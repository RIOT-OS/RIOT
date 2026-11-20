#![no_std]

use riot_wrappers::led::LED;
use riot_wrappers::riot_main;

riot_main!(main);

fn main() {
    // Using the `unchecked` methods because the loop rotates the blinking speeds through all LEDs
    // anyway -- so every speed will be on every LED, and that's easier to express if the number of
    // considered LEDs is constant.
    let mut led0 = LED::<0>::new_unchecked();
    let mut led1 = LED::<1>::new_unchecked();
    let mut led2 = LED::<2>::new_unchecked();
    let mut led3 = LED::<3>::new_unchecked();
    let mut led4 = LED::<4>::new_unchecked();
    let mut led5 = LED::<5>::new_unchecked();
    let mut led6 = LED::<6>::new_unchecked();
    let mut led7 = LED::<7>::new_unchecked();
    let mut leds: [&mut dyn switch_hal::ToggleableOutputSwitch<Error = _>; 8] = [
        &mut led0, &mut led1, &mut led2, &mut led3, &mut led4, &mut led5, &mut led6, &mut led7,
    ];
    loop {
        for i in 0..=255 {
            for (j, led) in leds.iter_mut().enumerate() {
                if (i ^ (i - 1)) & (1 << j) != 0 {
                    led.toggle().unwrap();
                }
            }
        }

        // The LSB blinking is probably way too fast; after a cycle, shift so we're seeing other
        // significances in the implemented LEDs. It's still *way* too fast for the naked eye.
        leds.rotate_left(1);
    }
}
