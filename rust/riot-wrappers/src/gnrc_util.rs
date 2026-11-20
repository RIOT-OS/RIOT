//! Experimental area for GNRC utility functions
//!
//! These are implemented directly in Rust and do not wrap any RIOT libraries, but seem useful at
//! least for the purpose of the author's experiments. It may turn out that they'd make nice
//! additions to the RIOT API, or are completely misguided.

#[cfg(riot_module_ipv6)]
use crate::gnrc::ipv6;
use crate::gnrc_pktbuf::{NotEnoughSpace, Pktsnip, Shared};
use crate::thread::KernelPID;

#[cfg(riot_module_gnrc_udp)]
use riot_sys::gnrc_nettype_t_GNRC_NETTYPE_UDP as GNRC_NETTYPE_UDP;

/// Trait of data structures that store all the information needed to respond to a Pktsnip in some
/// way; the data (typically address and port information) is copied into the trait implementation
/// and persisted there while the original snip is dropped and a new one is written.
///
/// This trait, in future, might also participate in the re-use of snips that are not dropped and
/// re-allocated but turned into responses in-place, but whether that makes sense here remains to
/// be seen.
pub trait RoundtripData: Sized {
    /// Read round trip data from an incoming packet.
    ///
    /// This returns something if the packet can fundamentally be responded to, which is usually
    /// the case.
    fn from_incoming(incoming: &Pktsnip<Shared>) -> Option<Self>;
    fn wrap(self, payload: Pktsnip<Shared>) -> Result<Pktsnip<Shared>, NotEnoughSpace>;
}

#[derive(Debug)]
pub struct NetifRoundtripData {
    // This would be more compact if we promised to never make UNKNOWN or ISR into KernelPIDs.
    pid: Option<KernelPID>,
}

impl RoundtripData for NetifRoundtripData {
    fn from_incoming(incoming: &Pktsnip<Shared>) -> Option<Self> {
        let header = incoming.netif_get_header()?;
        Some(NetifRoundtripData {
            pid: header.if_pid(),
        })
    }

    fn wrap(self, payload: Pktsnip<Shared>) -> Result<Pktsnip<Shared>, NotEnoughSpace> {
        Ok(match self.pid {
            None => payload,
            Some(pid) => payload
                .netif_hdr_builder()
                .without_link_layer_addresses()
                .with_if_pid(pid)
                .finish()?
                .into(),
        })
    }
}

#[cfg(riot_module_ipv6)]
#[derive(Debug)]
pub struct IPv6RoundtripDataFull<N: RoundtripData> {
    remote: ipv6::Address,
    local: ipv6::Address,
    // We "only" need the NetifRoundtripData if our destination address has a %interface part in it
    // -- which fortunately is a typical case during development, for otherwise that step might
    // easily be forgotten and IPv6RoundtripDataFull might be missing its next.
    next: N,
}

#[cfg(riot_module_ipv6)]
impl<N: RoundtripData> RoundtripData for IPv6RoundtripDataFull<N> {
    fn from_incoming(incoming: &Pktsnip<Shared>) -> Option<Self> {
        let ip = incoming.ipv6_get_header().unwrap();

        Some(Self {
            remote: *ip.src(),
            local: *ip.dst(),
            next: N::from_incoming(incoming)?,
        })
    }

    fn wrap(self, payload: Pktsnip<Shared>) -> Result<Pktsnip<Shared>, NotEnoughSpace> {
        self.next.wrap(
            payload
                .ipv6_hdr_build(Some(&self.local), Some(&self.remote))?
                .into(),
        )
    }
}

// It'd be nice to have a UDPRoundtripData (not full) that wouldn't need to store the local port
// (b/c it's usually known in the context), but how would that get past a .wrap()-ish API?
#[cfg(riot_module_gnrc_udp)]
#[derive(Debug)]
pub struct UDPRoundtripDataFull<N: RoundtripData> {
    remote: core::num::NonZeroU16,
    local: core::num::NonZeroU16,
    next: N,
}

#[cfg(riot_module_gnrc_udp)]
impl<N: RoundtripData> RoundtripData for UDPRoundtripDataFull<N> {
    fn from_incoming(incoming: &Pktsnip<Shared>) -> Option<Self> {
        let (src, dst) = incoming
            .search_type(GNRC_NETTYPE_UDP)
            .map(|s| {
                let hdr: &riot_sys::udp_hdr_t = unsafe { &*(s.data.as_ptr() as *const _) };
                (
                    u16::from_be_bytes(unsafe { (*hdr).src_port.u8_ }),
                    u16::from_be_bytes(unsafe { (*hdr).dst_port.u8_ }),
                )
            })
            .unwrap();

        Some(Self {
            // Taking RFC 768 literally, the remote port can easily be 0 if no responses are
            // expected. (Treating the local port the same for practical reasons; it won't be zero
            // at least if the data is from listening on a concrete UDP port).
            remote: src.try_into().ok()?,
            local: dst.try_into().ok()?,
            next: N::from_incoming(incoming)?,
        })
    }

    fn wrap(self, payload: Pktsnip<Shared>) -> Result<Pktsnip<Shared>, NotEnoughSpace> {
        self.next
            .wrap(payload.udp_hdr_build(self.local, self.remote)?.into())
    }
}
