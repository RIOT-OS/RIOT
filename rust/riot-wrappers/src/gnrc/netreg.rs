//! Registrations inside GNRC
//!
//! This module is split depending on the method of information passing: Methods that use the
//! classical message based information passing are placed directly here for the time being.
//! Callback based registration is available in the [callback] module, and is easier to use once
//! accepting that the callbacks need to be available statically.

#[cfg(riot_module_gnrc_netapi_callbacks)]
pub mod callback;

#[cfg(feature = "with_msg_v2")]
use core::mem::MaybeUninit;

#[cfg(feature = "with_msg_v2")]
use crate::error::NegativeErrorExt;

// Transmuting the pointer into a Pktsnip does the right thing by treating it as a smart
// pointer; dropping it decrements the refcount. (Otherwise we'd leak packets).
// What we drop here is what the netreg registration should consume: An owned (or lifetimed, if
// we move the handling into a closure) permission to send data to the indicated thread.
#[cfg(all(feature = "with_msg_v2", riot_module_gnrc_pktbuf))]
type PktsnipPort = crate::msg::v2::SendPort<
    crate::gnrc_pktbuf::Pktsnip<crate::gnrc_pktbuf::Shared>,
    { riot_sys::GNRC_NETAPI_MSG_TYPE_RCV as _ },
>;

/// Set up a netreg for a particular kind of messages
///
/// ## Roadmap
///
/// It might be convenient for this to return at some point (in case of short-lived network
/// services). Deregistration could be done and everything returned alright -- but the grant would
/// still be lost. This could be mitigated by accepting a 'static PktsnipPort or a cloneable version
/// thereof -- not that anything could still be recombined after that, but at least it could be
/// used once more (with the risk that messages from the old registration arrive in the new one,
/// which is wrong correctness-wise but safe because it'll still be a pointer to a pktsnip).
///
/// Any API rewrite would also replace the nettype/demux_ctx pair with a [FullDemuxContext].
#[cfg(all(feature = "with_msg_v2", riot_module_gnrc_pktbuf))]
pub fn register_for_messages<F: FnOnce() -> crate::never::Never>(
    grant: PktsnipPort,
    nettype: riot_sys::gnrc_nettype_t,
    demux_ctx: u32,
    f: F,
) -> ! {
    let mut entry = MaybeUninit::uninit();
    unsafe {
        riot_sys::gnrc_netreg_entry_init_pid(
            crate::inline_cast_mut(entry.as_mut_ptr()),
            demux_ctx,
            grant.destination().into(),
        )
    };
    let mut entry: riot_sys::gnrc_netreg_entry_t = unsafe { entry.assume_init() };

    (unsafe { riot_sys::gnrc_netreg_register(nettype, &mut entry) })
        .negative_to_error()
        .unwrap();

    f()
}

/// A combination of a GNRC net type and a demux context, as used in GNRC registrations.
pub struct FullDemuxContext {
    nettype: riot_sys::gnrc_nettype_t,
    demux_ctx: u32,
}

impl<'a> core::fmt::Debug for FullDemuxContext {
    fn fmt(&self, f: &mut ::core::fmt::Formatter) -> ::core::fmt::Result {
        // We could spare ourselves the trouble of manual implementation by storing nettype as
        // NetType, but that won't help with the demux_ctx == DEMUX_CTX_ALL case
        let mut stru = f.debug_struct("PktsnipPart");
        let mut stru = stru.field("nettype", &crate::gnrc::NetType(self.nettype));

        if self.demux_ctx == riot_sys::GNRC_NETREG_DEMUX_CTX_ALL {
            stru = stru.field("demux_ctx", &"all contexts");
        } else {
            stru = stru.field("demux_ctx", &format_args!("{:02x?}", self.demux_ctx));
        }

        stru.finish()
    }
}

impl FullDemuxContext {
    pub fn new_raw(nettype: riot_sys::gnrc_nettype_t, demux_ctx: u32) -> Self {
        Self { nettype, demux_ctx }
    }

    #[cfg(riot_module_gnrc_nettype_icmpv6)]
    pub fn new_icmpv6_echo(type_: super::icmpv6::EchoType) -> Self {
        Self {
            nettype: riot_sys::gnrc_nettype_t_GNRC_NETTYPE_ICMPV6,
            demux_ctx: u32::from(type_),
        }
    }
}
