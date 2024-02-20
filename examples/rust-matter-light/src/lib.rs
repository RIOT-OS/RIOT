// Copyright (C) 2024 Maik Lorenz
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]

use core::ffi::CStr;

use riot_wrappers::{
    riot_main, println, static_command, 
    stdio,
    shell,
    saul,
};

// periph_gpio module
use riot_wrappers::gpio::{GPIO, OutputGPIO, OutputMode, InputGPIO, InputMode};

// shell module
use riot_wrappers::shell::CommandList;

// saul module
use riot_wrappers::saul::{
    registration::{register_and_then, Drivable, Driver, Error, Registration},
    Class, ActuatorClass, Phydat, RegistryEntry, Unit
};

extern crate rust_riotmodules;

// User LED port and pin on used boards
const LED_PORT: u32 = 0;
const LED_PIN: u32 = 5;

// RGB LED port and pin
const RGB_PORT: u32 = 0;
const RGB_RED_PIN: u32 = 7;
const RGB_GREEN_PIN: u32 = 6;
const RGB_BLUE_PIN: u32 = 5;

fn get_output(port: u32, pin: u32) -> OutputGPIO {
    GPIO::from_port_and_pin(port, pin)
        .expect("Error configuring pin")
        .configure_as_output(OutputMode::Out)
        .expect("Error configuring GPIO output")
}

fn get_input(port: u32, pin: u32) -> InputGPIO {
    GPIO::from_port_and_pin(port, pin)
        .expect("Error configuring pin")
        .configure_as_input(InputMode::In)
        .expect("Error configuring GPIO input")
}

struct RgbLed{
    name: &'static str,
    portpin_red: (u32, u32),
    portpin_green: (u32, u32),
    portpin_blue: (u32, u32),
}

struct CurrentState {
    red: i16,
    green: i16,
    blue: i16,
}

impl Drivable for &RgbLed {
    const CLASS: Class = Class::Actuator(Some(ActuatorClass::LedRgb));
    const HAS_READ: bool = true;
    const HAS_WRITE: bool = true;

    fn read(self) -> Result<Phydat, Error> {
        let value = Phydat::new(&[
            unsafe { RGB_CURRENT.red },
            unsafe { RGB_CURRENT.green },
            unsafe { RGB_CURRENT.blue }],
            None,
            0
        );
        println!("Reading from RGB LED: {:?}", value);
        Ok(value)
    }

    fn write(self, data: &Phydat) -> Result<u8, Error> {
        println!("Writing {:?} to {}", data, self.name);
        let values = data.value();

        // obtain GPIO outputs for RGB LED's and set values
        let mut red = get_output(self.portpin_red.0, self.portpin_red.1);
        red.set_state(values[0] > 0);
        let mut green = get_output(self.portpin_green.0, self.portpin_green.1);
        green.set_state(values[1] > 0);
        let mut blue = get_output(self.portpin_blue.0, self.portpin_blue.1);
        blue.set_state(values[2] > 0);

        // save current RGB values
        unsafe {
            RGB_CURRENT.red = values[0];
            RGB_CURRENT.green = values[1];
            RGB_CURRENT.blue = values[2];
        }

        Ok(1)
    }
}

fn write_rgb_led(state: bool, red: u8, green: u8, blue: u8) {
    let mut success = false;
    
    // Find RGB LED at SAUL registry and write values to it
    RegistryEntry::all().for_each(|entry| {
            if let Some(Class::Actuator(Some(ActuatorClass::LedRgb))) = entry.type_() {
                let color = Phydat::new(&[red as i16, green as i16, blue as i16], None, 0);
                let _ = entry.write(color);
                success = true;
            }
        }
    );
    
    if !success {
        println!("RGB LED was not found in registry");
    }
}

fn cmd_onoff(_stdio: &mut stdio::Stdio, args: shell::Args<'_>) {
    let arg_state = args.get(1);
    match arg_state {
        Some(state_str) => {
            let new_state: bool;
            match state_str {
                "on" => new_state = true,
                "off" => new_state = false,
                _ => {
                    println!("Invalid argument for LED state (must be 'on' or 'off')");
                    return;
                }
            }
            if new_state {
                write_rgb_led(new_state, u8::MAX, u8::MAX, u8::MAX);
            } else {
                write_rgb_led(new_state, u8::MIN, u8::MIN, u8::MIN);
            }
        }
        None => {
            println!("Usage: onoff [on|off]");
        }
    }
}

static_command!(cmd_onoff, "onoff", "Turn the Color Light on or off", cmd_onoff);

static mut RGB_CURRENT: CurrentState = CurrentState {
    red: 0,
    green: 0,
    blue: 0
};

static RGB_LED_DRIVER: Driver<RgbLed> = Driver::new();

riot_main!(main);
fn main() {
    println!("Hello Matter on RIOT!");

    let mut board_led = get_output(LED_PORT, LED_PIN);
    board_led.set_low();

    let rgb_led = RgbLed{
        name: "Color Temperature Light",
        portpin_red: (RGB_PORT, RGB_RED_PIN),
        portpin_green: (RGB_PORT, RGB_GREEN_PIN),
        portpin_blue: (RGB_PORT, RGB_BLUE_PIN),
    };

    register_and_then(&RGB_LED_DRIVER, &rgb_led, Some(CStr::from_bytes_with_nul(b"Color Temperature Light\0").unwrap()), || {
        println!("RGB LED registered as SAUL actuator");
        shell::new().run_forever_providing_buf();
    });
}
