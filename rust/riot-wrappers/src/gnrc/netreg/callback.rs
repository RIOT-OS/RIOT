//! Callback based registration to a the netreg infrastructure

use core::mem::MaybeUninit;

use super::FullDemuxContext;
use crate::error::NegativeErrorExt;
use crate::gnrc_pktbuf as pktbuf;

/// Storage for everything that is needed to serve a registered GNRC netreg [Callback].
///
/// It can be created through [Default::default()]. It is used as a `&'static mut`
/// in [`register_static`], which is most easily obtained through the `static_cell` crate.
///
/// ## Internal invariants
///
/// When created, all fields are uninitialized; any registration that uses them places data in
/// there, and if it ever handed out the data structure again (usually it doesn't and consumes a
/// `&'static mut Slot`), they would be uninit again.
pub struct Slot<C>(
    MaybeUninit<riot_sys::gnrc_netreg_entry_t>,
    MaybeUninit<riot_sys::gnrc_netreg_entry_cbd_t>,
    MaybeUninit<C>,
);

impl<C> Default for Slot<C> {
    fn default() -> Self {
        Self(
            MaybeUninit::uninit(),
            MaybeUninit::uninit(),
            MaybeUninit::uninit(),
        )
    }
}

/// Callback trait for registration with netreg.
///
/// This is expressed as a trait rather than a FnMut because the callback generally needs to be
/// statically allocated, and a closure (which can not be named) can not.
pub trait Callback: Send {
    /// A network command to which the callback has been registered happened.
    ///
    /// This just takes a `&self` because multiple threads could concurrently create network events
    /// from different devices.
    fn called(&self, cmd: Command, snip: pktbuf::Pktsnip<pktbuf::Shared>);
}

/// Set up a `callback` for whenever a package matching `context` arrives.
///
/// This requires a statically allocated [`Slot`], as can conveniently be created by the caller
/// through the `static_cell` crate.
///
/// The callback's [Callback::called] method will be called whenever there is a packet is sent or
/// received that matches the given `context`.
pub fn register_static<C: Callback>(
    slot: &'static mut Slot<C>,
    callback: C,
    context: FullDemuxContext,
) {
    unsafe extern "C" fn c_callback<C: Callback>(
        cmd: u16,
        pkt: *mut riot_sys::gnrc_pktsnip_t,
        ctx: *mut crate::libc::c_void,
    ) {
        // unsafe: Constructed through the opposite cast, and API promises to deliver that value
        let callback = unsafe { &mut *(ctx as *mut C) };
        let cmd = match cmd as _ {
            riot_sys::GNRC_NETAPI_MSG_TYPE_RCV => Command::Receive,
            riot_sys::GNRC_NETAPI_MSG_TYPE_SND => Command::Send,
            _ => panic!("gnc_netreg_entry_cb_t precondition failed"),
        };
        // unsafe: Trusting the C API to produce a snip along with ownership
        let pkt = unsafe { pktbuf::Pktsnip::<pktbuf::Shared>::from_ptr(pkt) };
        callback.called(cmd, pkt)
    }

    slot.2.write(callback);

    slot.1.write(riot_sys::gnrc_netreg_entry_cbd_t {
        cb: Some(c_callback::<C>),
        ctx: slot.2.as_mut_ptr() as *mut crate::libc::c_void,
    });

    unsafe {
        riot_sys::inline::gnrc_netreg_entry_init_cb(
            crate::inline_cast_mut(slot.0.as_mut_ptr()),
            context.demux_ctx,
            crate::inline_cast_mut(slot.1.as_mut_ptr()),
        );

        riot_sys::gnrc_netreg_register(context.nettype, slot.0.as_mut_ptr())
            .negative_to_error()
            .unwrap();
    }
}

/// Values of the command argument of a netreg callback
#[non_exhaustive]
#[derive(Debug, PartialEq, Eq)]
pub enum Command {
    Receive,
    Send,
}
