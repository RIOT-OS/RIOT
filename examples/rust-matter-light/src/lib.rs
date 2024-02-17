// Copyright (C) 2024 Maik Lorenz
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]

use riot_wrappers::shell::CommandList;
use riot_wrappers::{
    riot_main, println, shell, static_command, stdio, saul,
    gpio::{OutputGPIO, OutputMode, GPIO},
};

extern crate rust_riotmodules;

// LED port and pin on used boards - adjust to your needs!
const LED_PORT: u32 = 0;
const LED_PIN: u32 = 5;

riot_main!(main);

fn get_rgb_led() -> OutputGPIO {
    GPIO::from_port_and_pin(LED_PORT, LED_PIN)
        .expect("Error configuring 'LD2' pin")
        .configure_as_output(OutputMode::Out)
        .expect("Error configuring 'LD2' LED")
}

fn cmd_echo(_stdio: &mut stdio::Stdio, args: shell::Args<'_>) {
    println!("Running args of run:");
    for a in args.iter() {
        println!("{:?}", a);
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
            let mut rgb_led = get_rgb_led();
            rgb_led.set_state(new_state);
        }
        None => {
            println!("Usage: onoff [on|off]");
        }
    }
}

static_command!(cmd_echo, "echo", "Print the arguments in separate lines", cmd_echo);
static_command!(cmd_onoff, "onoff", "Turn the LED on or off", cmd_onoff);


fn main() {
    println!("Hello Matter on RIOT!");

    let mut rgb_led = get_rgb_led();
    rgb_led.set_low();

    shell::new().run_forever_providing_buf();
}
