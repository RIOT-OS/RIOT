use embedded_nal_async::Ipv6Addr;
use log::info;
use riot_wrappers::error::{NegativeErrorExt, NumericError, EAGAIN};
use riot_wrappers::gnrc::Netif;
use rs_matter::error::ErrorCode;
use rs_matter::mdns::builtin::MDNS_IPV6_BROADCAST_ADDR;
use riot_wrappers::gnrc::netapi::join_multicast_v6;

extern "C" {
    fn init_constfs() -> i32;
}

pub fn init_vfs() -> Result<(), NumericError> {
    unsafe { init_constfs() }
        .negative_to_error()
        .map(|_| ())
}

#[inline(never)]
pub fn get_ipv6_address(ifc: &Netif) -> Option<Ipv6Addr> {
    let all_addresses = ifc.ipv6_addrs().ok()?;
    info!("Found {} addresses", all_addresses.len());

    // TODO: Find the Link-Local IPv6 address used for Matter communication
    // This address will be sent with DNS-SD Reponses
    let idx = if all_addresses.len() >= 2 { 1 } else { 0 };
    all_addresses.get(idx).map(|a| {
        let ipv6_raw = a.raw();
        Ipv6Addr::new(
            ((ipv6_raw[0] as u16) << 8) | ipv6_raw[1] as u16,
            ((ipv6_raw[2] as u16) << 8) | ipv6_raw[3] as u16,
            ((ipv6_raw[4] as u16) << 8) | ipv6_raw[5] as u16,
            ((ipv6_raw[6] as u16) << 8) | ipv6_raw[7] as u16,
            ((ipv6_raw[8] as u16) << 8) | ipv6_raw[9] as u16,
            ((ipv6_raw[10] as u16) << 8) | ipv6_raw[11] as u16,
            ((ipv6_raw[12] as u16) << 8) | ipv6_raw[13] as u16,
            ((ipv6_raw[14] as u16) << 8) | ipv6_raw[15] as u16,
        )
    })
}

#[inline(never)]
pub fn initialize_network() -> Result<(Ipv6Addr, u32), NumericError> {
    // Get first available interface
    let ifc: Netif = Netif::all().next().ok_or(ErrorCode::NoNetworkInterface)
        .map_err(|_| EAGAIN)?;

    // atm only for debugging: Check name and status of KernelPID
    let pid = ifc.pid();
    let ifc_name: &str = pid.get_name().unwrap_or("unknown");

    join_multicast_v6(pid, MDNS_IPV6_BROADCAST_ADDR).expect("Unable to join IPv6 mDNS Multicast group");
    info!("Joined IPV6 multicast group @ {:?}", MDNS_IPV6_BROADCAST_ADDR);

    // Get available IPv6 link-local address
    let ipv6 = get_ipv6_address(&ifc).ok_or(ErrorCode::StdIoError)
        .map_err(|_| EAGAIN)?;
    info!("Found network interface '{}' with IP {}", ifc_name, ipv6);
    Ok((ipv6, 0 as _))
}