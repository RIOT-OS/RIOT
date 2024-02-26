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
mod dev_att;

// external modules
use core::{ffi::CStr, borrow::Borrow, pin::pin};
use embedded_nal_async::{Ipv6Addr, Ipv4Addr, SocketAddr, UdpStack as _};
use static_cell::StaticCell;

// RIOT OS modules
extern crate rust_riotmodules;
use riot_wrappers::{
    riot_main, println, static_command, stdio,
    saul::registration::register_and_then, shell::{self, CommandList},
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

use riot_wrappers::gnrc::{Netif, ipv6::Address};

fn get_ipv6_addresses(ifc: &Netif, is_local_link: bool) -> Option<Ipv6Addr> {
    return match ifc.ipv6_addrs().unwrap().first() {
        Some(a) => {
            let ipv6_raw = a.raw();
            Some(Ipv6Addr::new(
                ((ipv6_raw[0] as u16) << 8) | ipv6_raw[1] as u16, 
                ((ipv6_raw[2] as u16) << 8) | ipv6_raw[3] as u16, 
                ((ipv6_raw[4] as u16) << 8) | ipv6_raw[5] as u16, 
                ((ipv6_raw[6] as u16) << 8) | ipv6_raw[7] as u16, 
                ((ipv6_raw[8] as u16) << 8) | ipv6_raw[9] as u16, 
                ((ipv6_raw[10] as u16) << 8) | ipv6_raw[11] as u16, 
                ((ipv6_raw[12] as u16) << 8) | ipv6_raw[13] as u16, 
                ((ipv6_raw[14] as u16) << 8) | ipv6_raw[15] as u16, 
            ))
        },
        None => None,
    };
}

#[inline(never)]
fn initialize_network() -> Result<(Ipv4Addr, Ipv6Addr, u32), MatterError> {
    println!("Init network...");

    let ipv4: Ipv4Addr = Ipv4Addr::UNSPECIFIED;
    let mut ipv6: Ipv6Addr = Ipv6Addr::UNSPECIFIED;

    // Get available network interface(s)
    println!("Found {} network interface(s)", Netif::all().count());
    let mut interfaces = Netif::all();
    
    // Get first available interface
    let ifc_0 = interfaces.nth(0);
    if ifc_0.is_none() {
        println!("ERROR: No network interface was found!");
        return Err(MatterError::new(rs_matter::error::ErrorCode::NoNetworkInterface));
    }
    let ifc_0 = ifc_0.unwrap();

    // Check name and status of KernelPID
    let pid = ifc_0.pid();
    let ifc_name: &str = pid.get_name().unwrap();
    println!("Thread status of {:?}: {:?}", pid.get_name().unwrap(), pid.status().unwrap());

    // Get available IPv6 link-local address
    let addresses = get_ipv6_addresses(&ifc_0, true);
    match addresses {
        Some(ipv6) => {
            println!(
                "Will use network interface {} with {}/{}",
                ifc_name, ipv4, ipv6
            );
            return Ok((ipv4, ipv6, 0 as _));
        },
        None => return Err(MatterError::new(rs_matter::error::ErrorCode::StdIoError)),
    }

    Ok((ipv4, ipv6, 0 as _))
}

fn handler<'a>(matter: &'a Matter<'a>) -> impl Metadata + NonBlockingHandler + 'a {
    (
        NODE,
        root_endpoint::handler(0, matter)
            .chain(
                1,
                descriptor::ID,
                descriptor::DescriptorCluster::new(*matter.borrow()),
            )
            .chain(
                1,
                cluster_on_off::ID,
                cluster_on_off::OnOffCluster::new(*matter.borrow()),
            ),
    )
}

async fn run_matter() -> Result<(), MatterError> {
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
    let (ipv4_addr, ipv6_addr, interface) = initialize_network()?;
    
    // 3. Initialize mDNS Service
    let mdns = MdnsService::new(
        0,
        "rs-matter-demo",
        ipv4_addr.octets(),
        Some((ipv6_addr.octets(), interface)),
        &dev_det,
        MATTER_PORT,
    );
    println!("mDNS initialized!");

    // 4. Start Matter
    let dev_att = dev_att::HardCodedDevAtt::new();
    let epoch = rs_matter::utils::epoch::dummy_epoch;
    let rand = rs_matter::utils::rand::dummy_rand;
    let matter = Matter::new(
        // vid/pid should match those in the DAC
        &dev_det,
        &dev_att,
        &mdns,
        epoch,
        rand,
        MATTER_PORT,
    );
    let handler = HandlerCompat(handler(&matter));

    // When using a custom UDP stack (e.g. for `no_std` environments), replace with a UDP socket bind + multicast join for your custom UDP stack
    // The returned socket should be splittable into two halves, where each half implements `UdpSend` and `UdpReceive` respectively
    static UDP_SOCKET: StaticCell<riot_sys::sock_udp_t> = StaticCell::new();
    let stack = riot_wrappers::socket_embedded_nal_async_udp::UdpStack::new(|| UDP_SOCKET.try_uninit());
    let (addr, socket) = stack
        .bind_single(MDNS_SOCKET_BIND_ADDR)
        .await
        .expect("Can't create a socket");
    println!("Bound socket to {:?} - {:?}", MDNS_SOCKET_BIND_ADDR, addr.ip());

    let mut udp_buffers = UdpBuffers::new();
    // TODO: 2nd parameter passed to mdns.run is not accepted because: 
    //  'the trait `UdpReceive` is not implemented for `&UnconnectedUdpSocket`'
    //let mut mdns_runner = pin!(mdns.run(&socket, &socket, &mut udp_buffers));

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
            let _ = rgb_led.write(data);
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

    static EXECUTOR: StaticCell<embassy_executor_riot::Executor> = StaticCell::new();
    let executor: &'static mut _ = EXECUTOR.init(embassy_executor_riot::Executor::new());
    executor.run(|spawner| {
        spawner.spawn(amain(spawner)).unwrap();
    });

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

use embedded_nal_async::UnconnectedUdp;

#[embassy_executor::task]
async fn amain(spawner: embassy_executor::Spawner) {
    let (ipv4_addr, ipv6_addr, interface) = initialize_network().unwrap();

    static UDP_SOCKET: StaticCell<riot_sys::sock_udp_t> = StaticCell::new();
    let stack = riot_wrappers::socket_embedded_nal_async_udp::UdpStack::new(|| UDP_SOCKET.try_uninit());
    let addr = SocketAddr::new("::".parse().unwrap(), 5683);
/*     let mut sock = stack
        .bind_multiple(addr)
        .await
        .expect("Can't create a socket"); */
    let (sock_addr, mut sock) = stack
        .bind_single(addr)
        .await
        .expect("Can't create a socket");
    println!("Local address: {:?}", &sock_addr);

    println!("Waiting for receiving UDP packet...");
    let mut buffer: &mut [u8] = &mut [0 as u8; 255 as usize];
    let res = sock.receive_into(buffer).await;
    match res {
        Ok((size, addr1, addr2)) => println!("UDP packet received"),
        Err(_) => println!("Error on UDP receive"),
    }

    let a = async {
        let mut delay = ztimer::Clock::msec();
        for i in 0..10 {
            delay.delay_ms(200);
            println!("First future: {}", i);
            embassy_futures::yield_now().await;
        }
    };
    let b = async {
        let mut delay = ztimer::Clock::msec();
        for i in 0..10 {
            delay.delay_ms(100);
            println!("Second future: {}", i);
            embassy_futures::yield_now().await;
        }
    };
    embassy_futures::join::join(
        a, b
    ).await;
}
