#![cfg(riot_module_gnrc_pktbuf)]

use riot_sys::gnrc_nettype_t;

use crate::gnrc_pktbuf::{Pktsnip, Shared};

/// Dispatch a packet to all listeners of the given nettype and demux context.
///
/// The return value indicates the number of recipients (which, unlike in the wrapped
/// gnrc_netapi_dispatch_send call, may be ignored if nondelivery of the packet is OK).
#[doc(alias = "gnrc_netapi_dispatch_send")]
pub fn dispatch_send(
    nettype: gnrc_nettype_t,
    demux_ctx: u32,
    pkt: impl Into<Pktsnip<Shared>>,
) -> i32 {
    let pkt = unsafe { pkt.into().to_ptr() };
    let subscribers = unsafe {
        riot_sys::gnrc_netapi_dispatch_send(nettype, demux_ctx, crate::inline_cast_mut(pkt))
    };
    if subscribers == 0 {
        unsafe { riot_sys::inline::gnrc_pktbuf_release(crate::inline_cast_mut(pkt)) };
    }
    subscribers
}
