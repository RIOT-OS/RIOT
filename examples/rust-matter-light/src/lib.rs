// Copyright (C) 2024 Maik Lorenz
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]
#[allow(warnings)]
#[allow(unused)]

// declare internal modules
pub mod gpio;
pub mod light;
pub mod saul_reg;
mod dev_att;
mod network;
mod persist;
mod tests;
mod logging;

// internal modules imports
use network::utils::{initialize_network};
use logging::init_logger;
use network::UdpSocketWrapper;

// core library
use core::{ffi::CStr, borrow::Borrow, pin::pin};
use core::cell::Cell;

// external crates
use static_cell::StaticCell;
use embedded_nal_async::{Ipv4Addr, UdpStack as _};
use embedded_hal_async::delay::DelayNs as _;
use embedded_alloc::Heap;
use log::{debug, info, error};

// RIOT OS modules
extern crate rust_riotmodules;
use riot_wrappers::{riot_main, println};
use riot_wrappers::shell::CommandList;

// rs-matter
extern crate rs_matter;
use rs_matter::{CommissioningData, MATTER_PORT};
use rs_matter::utils::select::EitherUnwrap as _;
use rs_matter::transport::network::{UdpReceive as _, UdpSend as _, UdpBuffers};
use rs_matter::transport::core::{PacketBuffers, MATTER_SOCKET_BIND_ADDR};
use rs_matter::core::Matter;
use rs_matter::data_model::{
    cluster_basic_information::BasicInfoConfig,
    cluster_on_off,
    device_types::DEV_TYPE_ON_OFF_LIGHT,
    objects::*,
    root_endpoint,
    system_model::descriptor
};
use rs_matter::data_model::cluster_on_off::OnOffCluster;
use rs_matter::error::Error;
use rs_matter::mdns::MdnsService;
use rs_matter::mdns::builtin::{
    MDNS_IPV4_BROADCAST_ADDR, MDNS_IPV6_BROADCAST_ADDR, MDNS_SOCKET_BIND_ADDR,
};
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

struct MyOnOffCluster {
    cluster: OnOffCluster,
    on: Cell<bool>,
}

impl MyOnOffCluster {
    fn new(cluster: OnOffCluster) -> Self {
        Self {
            cluster,
            on: Cell::new(false),
        }
    }
}

impl Handler for MyOnOffCluster {
    fn read(&self, attr: &AttrDetails, encoder: AttrDataEncoder) -> Result<(), Error> {
        info!("MyOnOffCluster - read: {:?}", attr);
        OnOffCluster::read(&self.cluster, attr, encoder)
    }

    fn write(&self, attr: &AttrDetails, data: AttrData) -> Result<(), Error> {
        info!("MyOnOffCluster - write: {:?}", attr);
        OnOffCluster::write(&self.cluster, attr, data)
    }

    fn invoke(&self, exchange: &Exchange, cmd: &CmdDetails, data: &TLVElement, encoder: CmdDataEncoder) -> Result<(), Error> {
        info!("MyOnOffCluster - invoke: cmd={:?}, data={:?}", cmd, data.u32());
        OnOffCluster::invoke(&self.cluster, exchange, cmd, data, encoder)
    }
}
impl NonBlockingHandler for MyOnOffCluster {}

// Handler for endpoint 1 (Descriptor + OnOff Cluster)
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
                //cluster_on_off::OnOffCluster::new(*matter.borrow()),
                MyOnOffCluster::new(cluster_on_off::OnOffCluster::new(*matter.borrow())),
            ),
    )
}

#[global_allocator]
static HEAP: Heap = Heap::empty();

riot_main!(main);

fn main() -> ! {
    {
        use core::mem::MaybeUninit;
        const HEAP_SIZE: usize = 1024 * 180;
        static mut HEAP_MEM: [MaybeUninit<u8>; HEAP_SIZE] = [MaybeUninit::uninit(); HEAP_SIZE];
        unsafe { HEAP.init(HEAP_MEM.as_ptr() as usize, HEAP_SIZE) }
    }

    init_logger().expect("Error initializing logger");

    info!("Hello Matter on RIOT!");

    let (ipv6_addr, interface) = initialize_network().expect("Error getting network interface and IP addresses");

    static MDNS: StaticCell<MdnsService> = StaticCell::new();
    let mdns_service: &'static MdnsService = MDNS.init(MdnsService::new(
        0,
        "rs-matter-demo",
        Ipv4Addr::UNSPECIFIED.octets(),
        Some((ipv6_addr.octets(), interface)),
        &DEV_DET,
        MATTER_PORT,
    ));

    // Get Device attestation (hard-coded atm)
    static DEV_ATT: StaticCell<dev_att::HardCodedDevAtt> = StaticCell::new();
    let dev_att: &'static dev_att::HardCodedDevAtt = DEV_ATT.init(dev_att::HardCodedDevAtt::new());

    // TODO: Provide own epoch and rand functions
    let epoch = rs_matter::utils::epoch::sys_epoch;
    let rand = rs_matter::utils::rand::sys_rand;

    static MATTER: StaticCell<Matter> = StaticCell::new();
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
        spawner.spawn(run_mdns(mdns_service)).unwrap();
        spawner.spawn(run_matter(matter)).unwrap();
        spawner.spawn(run_psm(matter)).unwrap();
    });

    let mut shell_thread = || {
        debug!("Shell is running");
        saul_reg::run_shell_with_saul();
    };
    loop { }
}

#[embassy_executor::task]
async fn run_mdns(mdns: &'static MdnsService<'_>) {
    // Create UDP socket and bind to port 5353
    static UDP_MDNS_SOCKET: StaticCell<riot_sys::sock_udp_t> = StaticCell::new();
    let udp_stack = riot_wrappers::socket_embedded_nal_async_udp::UdpStack::new(|| UDP_MDNS_SOCKET.try_uninit());
    let (mdns_addr, mut mdns_sock) = udp_stack
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
async fn run_matter(matter: &'static Matter<'_>) {
    // Create UDP socket and bind to port 5540
    static UDP_MATTER_SOCKET: StaticCell<riot_sys::sock_udp_t> = StaticCell::new();
    let udp_stack = riot_wrappers::socket_embedded_nal_async_udp::UdpStack::new(|| UDP_MATTER_SOCKET.try_uninit());
    let (matter_addr, mut matter_sock) = udp_stack
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
    let mut matter_runner = pin!(matter_runner);

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
async fn run_psm(matter: &'static Matter<'_>) {
    info!("Starting Persistence Manager....");
    // TODO: Develop own 'Persistence Manager' using RIOT OS modules
    let mut psm = persist::Psm::new(&matter).expect("Error creating PSM");
    let mut psm_runner = pin!(psm.run());
    match psm_runner.await {
        Ok(_) => {
            info!("Persistence Manager terminated without errors!");
        }
        Err(err) => {
            error!("Persistence Manager terminated with errors: {:?}", err);
        }
    }
}
