use core::ops::DerefMut as _;

use rs_matter::{self, error::ErrorCode, transport::network::{UdpReceive, UdpSend}};
use rs_matter::error::Error as MatterError;
use riot_wrappers::mutex::Mutex;
use riot_wrappers::socket_embedded_nal_async_udp::UnconnectedUdpSocket;
use riot_wrappers::println;
use riot_wrappers::gnrc::Netif;

use embedded_nal_async::{IpAddr, Ipv4Addr, Ipv6Addr, SocketAddr, SocketAddrV6, UdpStack as _, UnconnectedUdp};

pub struct RiotSocket{
    local_addr: SocketAddr,
    socket: Mutex<UnconnectedUdpSocket>,
}

impl RiotSocket {
    pub fn new(local_addr: SocketAddr, socket: UnconnectedUdpSocket) -> Self {
        Self {
            local_addr,
            socket: Mutex::new(socket)
        }
    }
}

impl UdpSend for RiotSocket {
    async fn send_to(&mut self, data: &[u8], addr: SocketAddr) -> Result<(), MatterError> {
        println!("[RiotSocket] send_to {:?}", &addr);
        if addr.is_ipv6() {
            println!("Is IPv6 address -> send!");
            let res = self.socket.lock().deref_mut().send(self.local_addr, addr, data).await;
            match res {
                Ok(_) => return Ok(()),
                Err(_) => return Err(MatterError::new(ErrorCode::StdIoError)),
            }
        }
        println!("Is IPv4 address -> don't send!");
        Ok(())
    }
}

impl UdpReceive for RiotSocket {
    async fn recv_from(&mut self, buffer: &mut [u8]) -> Result<(usize, SocketAddr), MatterError> {
        println!("[RiotSocket] receive_into...");
        let res = self.socket.lock().deref_mut().receive_into(buffer).await;
        match res {
            Ok((bytes_recvd, local_addr, remote_addr)) => Ok((bytes_recvd, remote_addr)),
            Err(e) => Err(MatterError::new(ErrorCode::StdIoError)),
        }
        
    }
}

impl UdpSend for &RiotSocket {
    async fn send_to(&mut self, data: &[u8], addr: SocketAddr) -> Result<(), MatterError> {
        println!("[RiotSocket] send_to {:?}", &addr);
        if addr.is_ipv6() {
            let res = self.socket.lock().deref_mut().send(self.local_addr, addr, data).await;
            match res {
                Ok(_) => {
                    println!("send_to success!");
                    return Ok(());
                },
                Err(_) => {
                    println!("send_to failure!");
                    return Err(MatterError::new(ErrorCode::StdIoError));
                },
            }
        }
        println!("Is IPv4 address -> don't send!");
        Ok(())
    }
}

impl UdpReceive for &RiotSocket {
    async fn recv_from(&mut self, buffer: &mut [u8]) -> Result<(usize, SocketAddr), MatterError> {
        println!("[RiotSocket] receive_into...");
        let res = self.socket.lock().deref_mut().receive_into(buffer).await;
        match res {
            Ok((bytes_recvd, local_addr, remote_addr)) => Ok((bytes_recvd, remote_addr)),
            Err(e) => Err(MatterError::new(ErrorCode::StdIoError)),
        }
    }
}

pub mod utils {
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
    pub fn initialize_network() -> Result<(Ipv4Addr, Ipv6Addr, u32), MatterError> {
        println!("Init network...");
    
        // Get available network interface(s)
        let mut interfaces = Netif::all();
        //println!("Found {} network interface(s)", interfaces.count());
        
        // Get first available interface
        let mut found_ifc: Option<Netif> = None;
        match interfaces.next() {
            Some(ifc) => found_ifc = Some(ifc),
            None => {
                println!("ERROR: No network interface was found!");
                return Err(MatterError::new(rs_matter::error::ErrorCode::NoNetworkInterface));
            },
        }
        let ifc = found_ifc.unwrap();
        println!("Network interface was found!");
    
        // atm only for debugging: Check name and status of KernelPID
        let pid = ifc.pid();
        let ifc_name: &str = pid.get_name().unwrap();
        println!("Thread status of {:?}: {:?}", pid.get_name().unwrap(), pid.status().unwrap());
    
        // We won't use IPv4 atm
        let ipv4: Ipv4Addr = Ipv4Addr::UNSPECIFIED;
        
        // Get available IPv6 link-local address
        match get_ipv6_address(&ifc) {
            Some(ipv6) => {
                println!("Will use network interface {} with {}/{}", ifc_name, ipv4, ipv6);
                Ok((ipv4, ipv6, 0 as _))
            },
            None => Err(MatterError::new(rs_matter::error::ErrorCode::StdIoError)),
        }
    }

    pub async fn test_udp(sock: &mut UnconnectedUdpSocket) {
        println!("Waiting for UDP packets...");
        let mut buffer: &mut [u8] = &mut [0 as u8; 255 as usize];
        let res = sock.receive_into(buffer).await;
        match res {
            Ok((bytes_recvd, _local_addr, remote_addr)) => {
                println!("Received {} bytes from {:?}", bytes_recvd, remote_addr);
                use core::str;
                let as_str = str::from_utf8(&buffer).unwrap().trim_matches('\0');
                println!("Received data: {:?}", as_str);
            },
            Err(_) => println!("error while receiving UDP data"),
        }
    }
    
    pub async fn test_udp_wrapper(sock: &mut RiotSocket) {
        println!("Waiting for UDP packets of Wrapper Socket...");
        let mut buffer: &mut [u8] = &mut [0 as u8; 255 as usize];
        let res = sock.recv_from(buffer).await;
        match res {
            Ok((bytes_recvd, remote_addr)) => {
                println!("Received {} bytes from {:?}", bytes_recvd, remote_addr);
                use core::str;
                let as_str = str::from_utf8(&buffer).unwrap().trim_matches('\0');
                println!("Received data: {:?}", as_str);
            },
            Err(_) => println!("error while receiving UDP data"),
        }
    }
}