// Copyright (C) 2024 Maik Lorenz
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]

// internal modules
pub mod gpio;
pub mod light;
pub mod saul_reg;

// external modules
use core::ffi::CStr;
use embedded_nal_async::{SocketAddr, UdpStack as _};
use static_cell::StaticCell;

// RIOT OS modules
extern crate rust_riotmodules;
use riot_wrappers::{
    riot_main, println, cstr::cstr, static_command, stdio, thread,
    saul::registration::register_and_then, shell::{self, CommandList},
    mutex::Mutex,
};
use riot_wrappers::socket_embedded_nal_async_udp::UdpStack;
use riot_wrappers::gnrc::{self, netreg};
use riot_wrappers::ztimer;
use embedded_hal::blocking::delay::DelayMs;

// rs-matter
use rs_matter;
use rs_matter::core::{CommissioningData, Matter};
use rs_matter::data_model::cluster_basic_information::BasicInfoConfig;
use rs_matter::data_model::cluster_on_off;
use rs_matter::data_model::device_types::DEV_TYPE_ON_OFF_LIGHT;
use rs_matter::data_model::objects::*;
use rs_matter::data_model::root_endpoint;
use rs_matter::data_model::system_model::descriptor;
use rs_matter::error::Error as MatterError;
use rs_matter::mdns::builtin::{
    MDNS_IPV4_BROADCAST_ADDR, MDNS_IPV6_BROADCAST_ADDR, MDNS_SOCKET_BIND_ADDR,
};
use rs_matter::mdns::MdnsService;
use rs_matter::transport::core::{PacketBuffers, MATTER_SOCKET_BIND_ADDR};
use rs_matter::transport::network::UdpBuffers;
use rs_matter::MATTER_PORT;


// Application-specific constants
// User LED port and pin on used boards
const LED_PORT: u32 = 0;
const LED_PIN: u32 = 5;

// RGB LED port and pin
const RGB_PORT: u32 = 0;
const RGB_RED_PIN: u32 = 7;
const RGB_GREEN_PIN: u32 = 6;
const RGB_BLUE_PIN: u32 = 5;

const NODE: Node<'static> = Node {
    id: 0,
    endpoints: &[
        root_endpoint::endpoint(0),
        Endpoint {
            id: 1,
            device_type: DEV_TYPE_ON_OFF_LIGHT,
            clusters: &[descriptor::CLUSTER, cluster_on_off::CLUSTER],
        },
    ],
};

// Matter constants
const MATTER_VID: u16 = 0xFFF1;
const MATTER_PID: u16 = 0x8000;
const MATTER_SERIAL: &str = "aabbccddd";
const MATTER_DEVICE_NAME: &str = "OnOff Light";
const MATTER_PRODUCT_NAME: &str = "Light123";
const MATTER_VENDOR_NAME: &str = "Vendor 123";

fn run_matter() -> Result<(), MatterError> {
    println!("Starting Matter...");
    // 1. Create node
    let dev_det = BasicInfoConfig {
        vid: MATTER_VID,
        pid: MATTER_PID,
        hw_ver: 2,
        sw_ver: 1,
        sw_ver_str: "1",
        serial_no: MATTER_SERIAL,
        device_name: MATTER_DEVICE_NAME,
        product_name: MATTER_PRODUCT_NAME,
        vendor_name: MATTER_VENDOR_NAME,
    };

    // 2. Initialize network

    // 3. Initialize mDNS Service

    // 4. Start Matter

    Ok(())
}

fn cmd_onoff(_stdio: &mut stdio::Stdio, args: shell::Args<'_>) {
    let arg_state = args.get(1);
    match arg_state {
        Some(state_str) => {
            let is_on: bool;
            match state_str {
                "on" => is_on = true,
                "off" => is_on = false,
                _ => {
                    println!("Invalid argument for LED state (must be 'on' or 'off')");
                    return;
                }
            }
            use riot_wrappers::saul::{Phydat, ActuatorClass};
            let rgb_led = saul_reg::find_actuator(ActuatorClass::LedRgb).expect("No registered RGB LED was found!");
            let data: Phydat;
            if is_on {
                data = Phydat::new(&[1, 1, 1], None, 0);
            } else {
                data = Phydat::new(&[0, 0, 0], None, 0);
            }
            rgb_led.write(data);
        }
        None => {
            println!("Usage: onoff [on|off]");
        }
    }
}

static_command!(
    cmd_onoff,
    "onoff",
    "Turn the Color Light on or off",
    cmd_onoff
);

riot_main!(main);
fn main() -> ! {
    println!("Hello Matter on RIOT!");

    let mut board_led = gpio::get_output(LED_PORT, LED_PIN);
    board_led.set_high();

    // TODO: At least spawning a task, creating socket and running async functions works
/*     static EXECUTOR: StaticCell<embassy_executor_riot::Executor> = StaticCell::new();
    let executor: &'static mut _ = EXECUTOR.init(embassy_executor_riot::Executor::new());
    executor.run(|spawner| {
        spawner.spawn(amain(spawner)).unwrap();
    }); */

    let rgb_led = saul_reg::RgbLed::new(
        "Color Temperature Light",
        (RGB_PORT, RGB_RED_PIN),
        (RGB_PORT, RGB_GREEN_PIN),
        (RGB_PORT, RGB_BLUE_PIN),
    );

    register_and_then(
        &saul_reg::RGB_LED_DRIVER,
        &rgb_led,
        Some(CStr::from_bytes_with_nul(b"Color Temperature Light\0").unwrap()),
        || {
            println!("RGB LED registered as SAUL actuator");
            shell::new().run_forever_providing_buf();
        },
    );
}

#[embassy_executor::task]
async fn amain(spawner: embassy_executor::Spawner) {
    static UDP_SOCKET: StaticCell<riot_sys::sock_udp_t> = StaticCell::new();
    let stack = riot_wrappers::socket_embedded_nal_async_udp::UdpStack::new(|| UDP_SOCKET.try_uninit());
    let mut sock = stack
        .bind_multiple(SocketAddr::new("::".parse().unwrap(), 5683))
        .await
        .expect("Can't create a socket");
    println!("UDP socket created!");

    let a = async {
        let mut delay = ztimer::Clock::msec();
        for i in 0..10 {
            delay.delay_ms(200);
            println!("First future: {}", i);
            embassy_futures::yield_now();
        }
    };
    let b = async {
        let mut delay = ztimer::Clock::msec();
        for i in 0..10 {
            delay.delay_ms(100);
            println!("Second future: {}", i);
            embassy_futures::yield_now();
        }
    };
    embassy_futures::join::join(
        a, b
    ).await;
}
