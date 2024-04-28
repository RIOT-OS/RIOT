use core::time::Duration;
use embedded_nal_async::Ipv6Addr;
use log::{debug, info};
use riot_wrappers::error::{NegativeErrorExt, NumericError, EAGAIN};
use riot_wrappers::gnrc::Netif;
use riot_wrappers::mutex::Mutex;
use rs_matter::error::ErrorCode;
use rs_matter::mdns::builtin::MDNS_IPV6_BROADCAST_ADDR;
use riot_wrappers::gnrc::netapi::join_multicast_v6;

// RIOT_EPOCH(2020) in seconds since UNIX Epoch
const RIOT_EPOCH_SECS: u64 = 1577833200;

extern "C" {
    fn get_seconds_since_riot_epoch() -> u32;
    fn get_milliseconds() -> u16;
    fn init_constfs() -> i32;
}

// Current UNIX Epoch - must be in ms precision
static CURRENT_DURATION: Mutex<Duration> = Mutex::new(Duration::from_secs(RIOT_EPOCH_SECS));

pub fn init_vfs() -> Result<(), NumericError> {
    unsafe { init_constfs() }
        .negative_to_error()
        .map(|_| ())
}

/// Get the current time as UNIX Epoch with milliseconds precision using periph_rtc (if supported) or ztimer module.
///
/// TODO: This should be moved to the `matter` module in riot-wrappers
pub fn sys_epoch() -> Duration {
    let mut duration = CURRENT_DURATION.lock();
    let riot_sec = unsafe { get_seconds_since_riot_epoch() } as u64;
    let riot_msec = unsafe { get_milliseconds() } as u32;
    let unix_sec = RIOT_EPOCH_SECS + riot_sec;
    debug!("UNIX epoch (sec.ms): {}.{}", unix_sec, riot_msec);
    let new_duration = Duration::new(unix_sec, riot_msec*1000000);
    // make sure that we always get a new timestamp in rare cases if sec and msec didn't change
    if *duration == new_duration {
        *duration = Duration::new(unix_sec, (riot_msec + 1)*1000000);
    } else {
        *duration = new_duration;
    }
    *duration
}

#[inline(never)]
pub fn get_ipv6_address(ifc: &Netif) -> Option<Ipv6Addr> {
    let all_addresses = ifc.ipv6_addrs().ok()?;
    info!("Found {} addresses", all_addresses.len());

    // TODO: Find a suitable IPv6 address for Matter communication
    // this address will be sent with DNS-SD Reponses
    let idx = if all_addresses.len() >= 2 { 1 } else { 0 };
    return match all_addresses.get(idx) {
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