// periph_gpio module
use riot_wrappers::gpio::{GPIO, OutputGPIO, OutputMode, InputGPIO, InputMode};


// Application-specific constants
// User LED port and pin on used boards
pub const LED_PORT: u32 = 0;
pub const LED_PIN: u32 = 5;

// RGB LED port and pin
pub const RGB_PORT: u32 = 0;
pub const RGB_RED_PIN: u32 = 7;
pub const RGB_GREEN_PIN: u32 = 6;
pub const RGB_BLUE_PIN: u32 = 5;


// TODO: Make GPIO's static members
pub fn get_output(port: u32, pin: u32) -> OutputGPIO {
    GPIO::from_port_and_pin(port, pin)
        .expect("Error configuring pin")
        .configure_as_output(OutputMode::Out)
        .expect("Error configuring GPIO output")
}

pub fn get_input(port: u32, pin: u32) -> InputGPIO {
    GPIO::from_port_and_pin(port, pin)
        .expect("Error configuring pin")
        .configure_as_input(InputMode::In)
        .expect("Error configuring GPIO input")
}

pub fn set_output(port: u32, pin: u32, on: bool) {
    get_output(port, pin).set_state(on);
}