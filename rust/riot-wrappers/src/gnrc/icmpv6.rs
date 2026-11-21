//! Components for interacting with ICMPv6 messages on GNRC

/// Type of an ICMPv6 Echo packet
///
/// Used both to build echo packets (which, admittedly, are mainly requests in
/// [Pktsnip::icmpv6_echo_build]) and for registering (mainly for responses) by giving a
/// `u32::from(t)` as the demux context for the ICMPv6 nettype in
/// [registration](crate::gnrc::netreg::register_for_messages).
#[derive(Debug, Copy, Clone)]
pub enum EchoType {
    Request = riot_sys::ICMPV6_ECHO_REQ as _,
    Reply = riot_sys::ICMPV6_ECHO_REP as _,
}

impl From<EchoType> for u32 {
    fn from(input: EchoType) -> u32 {
        input as _
    }
}

use crate::gnrc_pktbuf::{NotEnoughSpace, Pktsnip, Writable};

impl<'a> Pktsnip<Writable> {
    #[doc(alias = "gnrc_icmpv6_echo_build")]
    pub fn icmpv6_echo_build(
        type_: EchoType,
        id: u16,
        seq: u16,
        payload: &[u8],
    ) -> Result<Pktsnip<Writable>, NotEnoughSpace> {
        let snip = unsafe {
            riot_sys::gnrc_icmpv6_echo_build(
                type_ as _,
                id,
                seq,
                // cast: C function lacks const declaration
                payload.as_ptr() as *mut _,
                payload.len() as _,
            )
        };
        if snip == 0 as *mut _ {
            Err(NotEnoughSpace)
        } else {
            unsafe { Ok(Pktsnip::<Writable>::from_ptr(snip)) }
        }
    }
}
