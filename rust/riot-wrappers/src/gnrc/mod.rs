#[cfg(riot_module_gnrc_icmpv6)]
pub mod icmpv6;
#[cfg(riot_module_ipv6)]
pub mod ipv6;
pub mod netif;

pub mod netapi;
pub mod netreg;
#[cfg(riot_module_gnrc_ipv6_nib)]
pub mod nib;

use riot_sys::{gnrc_netif_iter, gnrc_netif_t};

use crate::thread::KernelPID;
use core::iter::Iterator;

// Could be made public on the long run, but will need proper constructors and is_... functions.
// Right now, this is just for pretty-printing.
pub(crate) struct NetType(pub(crate) riot_sys::gnrc_nettype_t);

impl core::fmt::Debug for NetType {
    fn fmt(&self, f: &mut ::core::fmt::Formatter) -> ::core::fmt::Result {
        match self.0 {
            // To be updated from the gnrc_nettype_t enum definition on demand
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_TX_SYNC => write!(f, "TX_SYNC"),
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_NETIF => write!(f, "NETIF"),
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_UNDEF => write!(f, "undefined"),
            #[cfg(riot_module_gnrc_nettype_gomach)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_GOMACH => write!(f, "GOMACH"),
            #[cfg(riot_module_gnrc_nettype_lwmac)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_LWMAC => write!(f, "LWMAC"),
            #[cfg(riot_module_gnrc_nettype_custom)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_CUSTOM => write!(f, "CUSTOM"),
            #[cfg(riot_module_gnrc_nettype_sixlowpan)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_SIXLOWPAN => write!(f, "SIXLOWPAN"),
            #[cfg(riot_module_gnrc_nettype_ipv6)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_IPV6 => write!(f, "IPV6"),
            #[cfg(riot_module_gnrc_nettype_ipv6_ext)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_IPV6_EXT => write!(f, "IPV6_EXT"),
            #[cfg(riot_module_gnrc_nettype_icmpv6)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_ICMPV6 => write!(f, "ICMPV6"),
            #[cfg(riot_module_gnrc_nettype_ccn)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_CCN => write!(f, "CCN"),
            #[cfg(riot_module_gnrc_nettype_ccn)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_CCN_CHUNK => write!(f, "CCN_CHUNK"),
            #[cfg(riot_module_gnrc_nettype_ndn)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_NDN => write!(f, "NDN"),
            #[cfg(riot_module_gnrc_nettype_tcp)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_TCP => write!(f, "TCP"),
            #[cfg(riot_module_gnrc_nettype_udp)]
            riot_sys::gnrc_nettype_t_GNRC_NETTYPE_UDP => write!(f, "UDP"),
            x if riot_sys::gnrc_nettype_t_GNRC_NETTYPE_UNDEF < x
                && x < riot_sys::gnrc_nettype_t_GNRC_NETTYPE_NUMOF =>
            {
                write!(f, "unknown ({})", x)
            }
            x => write!(f, "invalid ({})", x),
        }
    }
}

struct NetifIter {
    current: *const gnrc_netif_t,
}

impl Iterator for NetifIter {
    type Item = *const gnrc_netif_t;

    fn next(&mut self) -> Option<Self::Item> {
        self.current = unsafe { gnrc_netif_iter(self.current) };
        if self.current == 0 as *const gnrc_netif_t {
            None
        } else {
            Some(self.current)
        }
    }
}

/// A registered netif
///
/// (In particular, that means that the implementation can access its fields without any further
/// synchronization).
pub struct Netif(*const gnrc_netif_t);

impl Netif {
    #[doc(alias = "gnrc_netif_iter")]
    pub fn all() -> impl Iterator<Item = Netif> {
        (NetifIter {
            current: 0 as *const gnrc_netif_t,
        })
        .map(Netif)
    }

    #[doc(alias = "gnrc_netif_get_by_pid")]
    pub fn by_pid(pid: KernelPID) -> Option<Self> {
        const NULL: *mut riot_sys::gnrc_netif_t = core::ptr::null_mut();
        // Not using as_ref: We can't guarantee that even for the short period between we're making
        // it into a reference and casting it back to a pointer again, it is not used by anyone
        // else
        match unsafe { riot_sys::gnrc_netif_get_by_pid(pid.into()) } {
            NULL => None,
            x => Some(Netif(x)),
        }
    }

    pub fn pid(&self) -> KernelPID {
        KernelPID(unsafe { (*self.0).pid })
    }

    pub fn l2addr(&self) -> &[u8] {
        unsafe { &(&(&(*self.0).l2addr)[..(*self.0).l2addr_len as usize]) }
    }
}
