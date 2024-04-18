// Copyright (C) 2024 Maik Lorenz
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]
#![feature(type_alias_impl_trait)]

// declare internal modules
mod dev_att;
mod network;
#[allow(dead_code)]
mod rgb_led;
#[allow(unused)]
mod persist;
mod logging;
// TODO: Enable attribute or feature to be able to run integration tests inside this module under RIOT OS
#[allow(unused)]
mod tests;
mod utils;

// internal modules imports
use network::UdpSocketWrapper;
use network::utils::initialize_network;
use logging::init_logger;
use dev_att::HardCodedDevAtt;

// core library
use core::{borrow::Borrow, pin::pin};
use core::cell::Cell;

// external crates
use static_cell::StaticCell;
use embedded_nal_async::{Ipv4Addr, UdpStack as _};
use embedded_alloc::Heap;
use embedded_hal::delay::DelayNs as _;
use log::{debug, info, error, LevelFilter, warn};

// RIOT OS modules
extern crate rust_riotmodules;

use riot_wrappers::{riot_main, static_command};
use riot_wrappers::ztimer;
use riot_wrappers::thread;
use riot_wrappers::mutex::Mutex;
use riot_wrappers::cstr::cstr;
use riot_wrappers::shell::{self, CommandList};
use riot_wrappers::saul::{ActuatorClass, Class, Phydat, RegistryEntry};

// rs-matter
#[allow(unused_variables)]
#[allow(dead_code)]
extern crate rs_matter;

use rs_matter::{CommissioningData, MATTER_PORT};
use rs_matter::transport::network::UdpBuffers;
use rs_matter::transport::core::{PacketBuffers, MATTER_SOCKET_BIND_ADDR};
use rs_matter::core::Matter;
use rs_matter::data_model::{
    cluster_basic_information::BasicInfoConfig,
    cluster_on_off,
    device_types::DEV_TYPE_ON_OFF_LIGHT,
    objects::*,
    root_endpoint,
    system_model::descriptor,
};
use rs_matter::data_model::cluster_on_off::{Commands, OnOffCluster};
use rs_matter::error::Error;
use rs_matter::mdns::MdnsService;
use rs_matter::mdns::builtin::MDNS_SOCKET_BIND_ADDR;
use rs_matter::secure_channel::spake2p::VerifierData;
use rs_matter::tlv::TLVElement;
use rs_matter::transport::exchange::Exchange;

// Node object with endpoints supporting device type 'OnOff Light'
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

#[allow(dead_code)]
struct OnOffHandler {
    cluster: OnOffCluster,
    on: Cell<bool>,
}

impl OnOffHandler {
    fn new(cluster: OnOffCluster) -> Self {
        Self {
            cluster,
            on: Cell::new(false),
        }
    }
}

impl Handler for OnOffHandler {
    fn read(&self, attr: &AttrDetails, encoder: AttrDataEncoder) -> Result<(), Error> {
        info!("OnOffCluster: Read attribute {:#04x} @ Endpoint {}", attr.attr_id, attr.endpoint_id);
        self.cluster.read(attr, encoder)
    }

    fn write(&self, attr: &AttrDetails, data: AttrData) -> Result<(), Error> {
        info!("OnOffCluster: Write attribute {:#04x} @ Endpoint {}", attr.attr_id, attr.endpoint_id);
        self.cluster.write(attr, data)
    }

    fn invoke(&self, exchange: &Exchange, cmd: &CmdDetails, data: &TLVElement, encoder: CmdDataEncoder) -> Result<(), Error> {
        info!("OnOffCluster: Invoke cmd {:#02x} (data: {}) @ Endpoint {}", cmd.cmd_id, data.u32().unwrap_or(0), cmd.endpoint_id);
        // Handle command by ID -> 0x00: Off, 0x01: On, 0x02: Toggle
        match cmd.cmd_id.try_into()? {
            Commands::On => led_onoff(true),
            Commands::Off => led_onoff(false),
            Commands::Toggle => {
                let new_state = !self.on.get();
                self.on.set(new_state);
                led_onoff(new_state);
            }
            Commands::OffWithEffect => info!("OffWithEffect ..."),
            Commands::OnWithRecallGlobalScene => info!("OnWithRecallGlobalScene ..."),
            Commands::OnWithTimedOff => info!("OnWithTimedOff ..."),
            //_ => { warn!("Unsupported command by application -> ignored!"); }
        }
        self.cluster.invoke(exchange, cmd, data, encoder)
    }
}

impl NonBlockingHandler for OnOffHandler {}

// Handler for endpoints 0 (Root) and 1 (Descriptor + OnOff Cluster)
fn matter_handler<'a>(matter: &'a Matter<'a>) -> impl Metadata + NonBlockingHandler + 'a {
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
                OnOffHandler::new(OnOffCluster::new(*matter.borrow())),
            ),
    )
}

#[global_allocator]
static HEAP: Heap = Heap::empty();
static SHELL_THREAD_STACK: Mutex<[u8; 5120]> = Mutex::new([0; 5120]);
static MATTER_THREAD_STACK: Mutex<[u8; 50000]> = Mutex::new([0; 50000]);
static DEV_DET: BasicInfoConfig = BasicInfoConfig {
    vid: 0xFFF1,
    pid: 0x8000,
    hw_ver: 2,
    sw_ver: 1,
    sw_ver_str: "1",
    serial_no: "aabbccddd",
    device_name: "OnOff Light",
    product_name: "Light123",
    vendor_name: "Vendor 123",
};
static MDNS: StaticCell<MdnsService> = StaticCell::new();
static DEV_ATT: StaticCell<HardCodedDevAtt> = StaticCell::new();
static MATTER: StaticCell<Matter> = StaticCell::new();

// Set this to the name of the LED, which should be controlled using SAUL registry
const ONOFF_LED_NAME: &str = "LD2(blue)";

riot_main!(main);

fn led_onoff(on: bool) {
    // Iterate through all SAUL registry entries and set all switches to the desired state
    let mut success = false;
    RegistryEntry::all().for_each(|entry| {
        if matches!(entry.type_(), Some(Class::Actuator(Some(ActuatorClass::Switch))))
            && entry.name().map_or(false, |name| name == ONOFF_LED_NAME) {
            // Found LED in SAUL -> turn on/off
            let data = Phydat::new(&[on as i16], None, 0);
            entry.write(data).expect("Error while trying to set LED");
            info!("LED {} was set to {:?}", entry.name().unwrap_or("n/a"), on);
            success = true;
        }
    });
    warn!("LED {} was not found in SAUL registry!", ONOFF_LED_NAME);
}

fn cmd_matter(_stdio: &mut riot_wrappers::stdio::Stdio, _args: shell::Args<'_>) {
    // TODO: Support some useful commands for this Matter node or interacting with other nodes
    info!("Endpoint Info:");
    NODE.endpoints.into_iter().for_each(|ep| {
        info!("Endpoint No. {} - DeviceType={:#04x}", ep.id, ep.device_type.dtype);
        ep.clusters.into_iter().for_each(|cluster| {
            info!("  Cluster ID={:#04x}", cluster.id);
            cluster.attributes.into_iter().for_each(|attr| info!("    Attribute {:#04x}", attr.id))
        });
    });
}

static_command!(matter, "matter", "Interact with Matter devices", cmd_matter);

fn run_shell() -> Result<(), ()> {
    let mut shell = shell::new();
    shell.run_forever_providing_buf()
}

fn run_matter() -> Result<(), ()> {
    let (ipv6_addr, interface) = initialize_network()
        .expect("Error getting network interface and IP addresses");

    let mdns_service: &'static MdnsService = MDNS.init(MdnsService::new(
        0,
        "riot-matter-demo",
        Ipv4Addr::UNSPECIFIED.octets(),
        Some((ipv6_addr.octets(), interface)),
        &DEV_DET,
        MATTER_PORT,
    ));

    // Get Device attestation (hard-coded atm)
    let dev_att: &'static HardCodedDevAtt = DEV_ATT.init(HardCodedDevAtt::new());

    // TODO: Provide own epoch and rand functions
    let epoch = utils::sys_epoch;
    let rand = utils::sys_rand;

    let matter: &'static Matter = MATTER.init(Matter::new(
        // vid/pid should match those in the DAC
        &DEV_DET,
        dev_att,
        mdns_service,
        epoch,
        rand,
        MATTER_PORT,
    ));

    info!("Starting all services...");
    static EXECUTOR: StaticCell<embassy_executor_riot::Executor> = StaticCell::new();
    let executor: &'static mut _ = EXECUTOR.init(embassy_executor_riot::Executor::new());
    executor.run(|spawner| {
        spawner.spawn(mdns_task(mdns_service)).unwrap();
        spawner.spawn(matter_task(matter)).unwrap();

        // Run PersistenceManager only if 'psm' feature is activated
        #[cfg(feature = "psm")]
        spawner.spawn(psm_task(matter)).unwrap();
    });
}

fn main() {
    {
        use core::mem::MaybeUninit;
        const HEAP_SIZE: usize = 4192;
        static mut HEAP_MEM: [MaybeUninit<u8>; HEAP_SIZE] = [MaybeUninit::uninit(); HEAP_SIZE];
        unsafe { HEAP.init(HEAP_MEM.as_ptr() as usize, HEAP_SIZE) }
    }

    init_logger(LevelFilter::Info).expect("Error initializing logger");
    let mut clock = ztimer::Clock::msec();
    clock.delay_ms(1000);

    info!("Hello Matter on RIOT!");

    use core::mem::size_of;
    info!("Matter memory usage: UdpBuffers={}, PacketBuffers={}, \
        MdnsService={}, Matter={}",
        size_of::<UdpBuffers>(),
        size_of::<PacketBuffers>(),
        size_of::<MdnsService>(),
        size_of::<Matter>(),
    );

    // Run threads: Matter and RIOT Shell
    let mut matter_thread_main = || run_matter().unwrap();
    let mut matter_thread_stacklock = MATTER_THREAD_STACK.lock();

    let mut shell_thread_main = || run_shell().unwrap();
    let mut shell_thread_stacklock = SHELL_THREAD_STACK.lock();

    thread::scope(|threadscope| {
        let matter_thread = threadscope.spawn(
            matter_thread_stacklock.as_mut(),
            &mut matter_thread_main,
            cstr!("matter"),
            (riot_sys::THREAD_PRIORITY_MAIN - 2) as _,
            (riot_sys::THREAD_CREATE_STACKTEST) as _,
        ).expect("Failed to spawn Matter thread");
        info!("MATTER thread spawned as {:?} ({:?}), status {:?}", matter_thread.pid(), matter_thread.pid().get_name(), matter_thread.status());

        let shell_thread = threadscope.spawn(
            shell_thread_stacklock.as_mut(),
            &mut shell_thread_main,
            cstr!("shell"),
            (riot_sys::THREAD_PRIORITY_MAIN - 1) as _,
            (riot_sys::THREAD_CREATE_STACKTEST) as _,
        ).expect("Failed to spawn shell thread");
        info!("SHELL thread spawned as {:?} ({:?}), status {:?}", shell_thread.pid(), shell_thread.pid().get_name(), shell_thread.status());

        loop {
            thread::sleep();
        }
    });
}

#[embassy_executor::task]
async fn mdns_task(mdns: &'static MdnsService<'_>) {
    // Create UDP socket and bind to port 5353
    static UDP_MDNS_SOCKET: StaticCell<riot_sys::sock_udp_t> = StaticCell::new();
    let udp_stack = riot_wrappers::socket_embedded_nal_async_udp::UdpStack::new(|| UDP_MDNS_SOCKET.try_uninit());
    let (mdns_addr, mdns_sock) = udp_stack
        .bind_single(MDNS_SOCKET_BIND_ADDR)
        .await
        .expect("Can't create a socket");
    let socket = UdpSocketWrapper::new(mdns_addr, mdns_sock);
    debug!("Created UDP socket for mDNS at {:?}", &mdns_addr);

    // Finally run the MDNS service
    let mut mdns_udp_buffers = UdpBuffers::new();
    let mdns_runner = pin!(mdns.run(
        &socket,
        &socket,
        &mut mdns_udp_buffers)
    );

    debug!("Starting MDNS....");
    match mdns_runner.await {
        Ok(_) => {
            info!("MDNS terminated without errors!");
        }
        Err(err) => {
            error!("MDNS terminated with error: {:?}", err);
        }
    }
}

#[embassy_executor::task]
async fn matter_task(matter: &'static Matter<'_>) {
    // Create UDP socket and bind to port 5540
    static UDP_MATTER_SOCKET: StaticCell<riot_sys::sock_udp_t> = StaticCell::new();
    let udp_stack = riot_wrappers::socket_embedded_nal_async_udp::UdpStack::new(|| UDP_MATTER_SOCKET.try_uninit());
    let (matter_addr, matter_sock) = udp_stack
        .bind_single(MATTER_SOCKET_BIND_ADDR)
        .await
        .expect("Can't create a socket");
    let socket = UdpSocketWrapper::new(matter_addr, matter_sock);
    debug!("Created UDP socket for Matter at {:?}", &matter_addr);

    let handler = HandlerCompat(matter_handler(&matter));

    let mut matter_udp_buffers = UdpBuffers::new();
    let mut matter_packet_buffers = PacketBuffers::new();

    // Finally create the Matter service and run on port 5540/UDP
    let matter_runner = pin!(matter.run(
        &socket,
        &socket,
        &mut matter_udp_buffers,
        &mut matter_packet_buffers,
        CommissioningData {
            // TODO: Hard-coded for now
            verifier: VerifierData::new_with_pw(123456, *matter.borrow()),
            discriminator: 250,
        },
        &handler)
    );
    let matter_runner = pin!(matter_runner);

    info!("Starting Matter...");
    match matter_runner.await {
        Ok(_) => {
            info!("Matter service terminated without errors!");
        }
        Err(err) => {
            error!("Matter service terminated with error: {:?}", err);
        }
    }
}

#[embassy_executor::task]
async fn psm_task(matter: &'static Matter<'_>) {
    info!("Starting Persistence Manager....");
    // TODO: Develop own 'Persistence Manager' using RIOT OS modules
    let mut psm = persist::Psm::new(&matter).expect("Error creating PSM");
    let psm_runner = pin!(psm.run());
    match psm_runner.await {
        Ok(_) => {
            info!("Persistence Manager terminated without errors!");
        }
        Err(err) => {
            error!("Persistence Manager terminated with errors: {:?}", err);
        }
    }
}
