use embedded_nal_async::{SocketAddr, UdpStack, UnconnectedUdp};
use log::{debug, error, info, LevelFilter};
use riot_wrappers::socket_embedded_nal_async_udp::UnconnectedUdpSocket;
use rs_matter::error::Error;
use rs_matter::transport::core::MATTER_SOCKET_BIND_ADDR;
use rs_matter::transport::network::{UdpReceive, UdpSend};
use static_cell::StaticCell;
use core::str;
use riot_wrappers::error::{EAGAIN, NumericError};
use riot_wrappers::matter::{init_logger, UdpSocketWrapper};
use crate::utils::initialize_network;

// Set this as entry point with 'riot_main!' macro to run 'integration tests'
fn run_tests() -> ! {
    init_logger(LevelFilter::Debug).expect("Error initializing logger");
    static EXECUTOR: StaticCell<embassy_executor_riot::Executor> = StaticCell::new();
    let executor: &'static mut _ = EXECUTOR.init(embassy_executor_riot::Executor::new());
    executor.run(|spawner| {
        spawner.spawn(test_udp()).unwrap();
    });
}

async fn udp_send<S>(mut send: S, data: &mut [u8], remote_addr: SocketAddr) -> Result<(), NumericError>
    where S: UdpSend {
    //let mut buffer: &mut [u8] = &mut [0u8; 255usize];
    return match send.send_to(data, remote_addr).await {
        Ok(_) => {
            info!("Sent echo!");
            Ok(())
        }
        Err(_) => {
            error!("Error sending echo!");
            Err(EAGAIN)
        }
    }
}

async fn udp_receive<R>(mut recv: R) -> Result<(SocketAddr, [u8; 255], usize), NumericError>
    where R: UdpReceive
{
    let mut buffer: [u8; 255] = [0u8; 255usize];
    return match recv.recv_from(&mut buffer).await {
        Ok((bytes_recvd, remote_addr)) => {
            info!("{} bytes received from {:?}", bytes_recvd, remote_addr);
            let as_str = str::from_utf8(&buffer).unwrap().trim_matches('\0');
            debug!("Received data: {:?}", as_str);
            debug!("as bytes: {:?}", &buffer[..bytes_recvd]);
            Ok((remote_addr, buffer, bytes_recvd))
        }
        Err(err) => {
            error!("Error during receive: {:?}", err);
            Err(EAGAIN)
        }
    }
}

#[embassy_executor::task]
async fn test_udp() {
    let (ipv6_addr, interface) = initialize_network().expect("Error getting network interface and IP addresses");

    static UDP_MDNS_SOCKET: StaticCell<riot_sys::sock_udp_t> = StaticCell::new();
    let udp_stack = riot_wrappers::socket_embedded_nal_async_udp::UdpStack::new(|| UDP_MDNS_SOCKET.try_uninit());
    let (addr, mut sock) = udp_stack
        .bind_single(MATTER_SOCKET_BIND_ADDR)
        .await
        .expect("Can't create a socket");
    let mut socket = &UdpSocketWrapper::new(addr, sock);
    info!("Listening on [{:?}]:5540", &ipv6_addr);

    let mut buffer: [u8; 255] =[0u8; 255usize];
    loop {
        info!("Waiting for incoming packets...");
        match udp_receive(socket).await {
            Ok((remote_addr, mut data, len)) => {
                info!("{} bytes received from {:?}", len, remote_addr);
                let as_str = str::from_utf8(&data).unwrap().trim_matches('\0');
                debug!("Received data: {:?}", as_str);
                debug!("as bytes: {:?}", &data[..len]);
                udp_send(socket, &mut data[..len], remote_addr).await.expect("error sending response");
            }
            Err(err) => {
                error!("Error during receive: {:?}", err);
            }
        }
    }
}