use rs_matter::error::Error as MatterError;
use riot_wrappers::gnrc::Netif;
use embedded_nal_async::{Ipv6Addr, SocketAddr, UnconnectedUdp};

use embassy_futures::select::{Either, select};
use riot_wrappers::mutex::Mutex;
use riot_wrappers::ztimer;
use riot_wrappers::socket_embedded_nal_async_udp::UnconnectedUdpSocket;
use rs_matter::error::{Error, ErrorCode};
use rs_matter::transport::network::{UdpReceive, UdpSend};
use embassy_sync::{
    signal::Signal,
    blocking_mutex::raw::NoopRawMutex
};
use embedded_hal_async::delay::DelayNs;
use log::{debug, error, warn};

pub type Notification = Signal<NoopRawMutex, ()>;

pub struct UdpSocketWrapper {
    local_addr: SocketAddr,
    socket: Mutex<UnconnectedUdpSocket>,
    release_socket_notification: Notification,
    socket_released_notification: Notification,
}

impl UdpSocketWrapper {
    pub fn new(local_addr: SocketAddr, socket: UnconnectedUdpSocket) -> Self {
        Self {
            local_addr,
            socket: Mutex::new(socket),
            release_socket_notification: Notification::new(),
            socket_released_notification: Notification::new(),
        }
    }
}

impl UdpSend for &UdpSocketWrapper {
    async fn send_to(&mut self, data: &[u8], addr: SocketAddr) -> Result<(), Error> {
        debug!("(UDP) sending {} bytes to {:?}", data.len(), &addr);
        if addr.is_ipv4() {
            // IPv4 not supported on RIOT OS
            warn!("(UDP) Is IPv4 -> ignore send!");
            return Ok(());
        }
        // tell receiver to release mutex within max. 10ms
        self.release_socket_notification.signal(());
        ztimer::Delay.delay_ms(5).await;
        let mut sock = self.socket.try_lock().expect("receiver should have ensured that this mutex is free");
        sock.send(self.local_addr, addr, data)
            .await
            .map_err(|_| Error::new(ErrorCode::StdIoError))?;
        // tell receiver sending is finished
        drop(sock);
        self.socket_released_notification.signal(());
        Ok(())
    }
}

impl UdpReceive for &UdpSocketWrapper {
    async fn recv_from(&mut self, buffer: &mut [u8]) -> Result<(usize, SocketAddr), Error> {
        loop {
            let mut sock = self.socket.try_lock().expect("sender should have ensured that this mutex is free");
            match select(
                self.release_socket_notification.wait(),
                sock.receive_into(buffer)
            ).await {
                Either::First(_) => {
                    // give up mutex for sender
                    drop(sock);
                    // ... and wait until available again
                    self.socket_released_notification.wait().await;
                    continue;
                }
                Either::Second(res) => {
                    match res {
                        Ok((bytes_recvd, local_addr, remote_addr)) => {
                            debug!("(UDP) received {} bytes from {:?} to {:?}", bytes_recvd, &remote_addr, &local_addr);
                            if remote_addr.is_ipv4() {
                                warn!("(UDP) Is IPv4 -> ignoring receive data!");
                                return Ok((bytes_recvd, remote_addr));
                            }
                        }
                        Err(_) => { error!("Error during UDP receive!"); }
                    }
                    // return receive result
                    let (bytes_recvd, remote_addr) = res.map(|(bytes_recvd, _, remote_addr)|
                        (bytes_recvd, remote_addr)
                    ).map_err(|_| Error::new(ErrorCode::StdIoError))?;
                    return Ok((bytes_recvd, remote_addr));
                }
            }
        }
    }
}

pub mod utils {
    use log::{debug, error, info};
    use super::*;

    pub fn get_ipv6_address(ifc: &Netif) -> Option<Ipv6Addr> {
        let all_addresses = ifc.ipv6_addrs();
        if all_addresses.is_err() {
            return None;
        }
    
        return match all_addresses.unwrap().first() {
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
    pub fn initialize_network() -> Result<(Ipv6Addr, u32), MatterError> {
        // Get available network interface(s)
        let mut interfaces = Netif::all();
        
        // Get first available interface
        let ifc: Netif = match interfaces.next() {
            Some(ifc) => Ok(ifc),
            None => {
                error!("ERROR: No network interface was found!");
                Err(MatterError::new(ErrorCode::NoNetworkInterface))
            },
        }?;

        // atm only for debugging: Check name and status of KernelPID
        let pid = ifc.pid();
        let ifc_name: &str = pid.get_name().unwrap();
        debug!("Kernel PID status of network interface {:?}: {:?}", pid.get_name().unwrap(), pid.status().unwrap());
    
        // Get available IPv6 link-local address
        match get_ipv6_address(&ifc) {
            Some(ipv6) => {
                info!("Found network interface {} with IP {}", ifc_name, ipv6);
                Ok((ipv6, 0 as _))
            },
            None => Err(MatterError::new(ErrorCode::StdIoError)),
        }
    }
}