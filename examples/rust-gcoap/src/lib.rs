// Copyright (C) 2020 Christian Amsüss
//
// This file is subject to the terms and conditions of the GNU Lesser
// General Public License v2.1. See the file LICENSE in the top level
// directory for more details.
#![no_std]

use riot_wrappers::{riot_main, println};
use riot_wrappers::{gcoap, thread, ztimer, gnrc};

use coap_handler_implementations::{ReportingHandlerBuilder, HandlerBuilder};

extern crate rust_riotmodules;

riot_main!(main);

mod suit {

use riot_wrappers::error::{NegativeErrorExt, NumericError, ENOMEM};

/// Error returned when attempting to obtain a new [ManifestLock], but it is impossible because the
/// requested size is too large, the SUIT worker is currently active, or both.
#[derive(Debug)]
pub enum ManifestLockError {
    Busy,
    TooLarge(usize)
}

/// Memory allocated for an incoming SUIT manifest transfer.
///
/// Leaking it leaks memory on the SUIT worker thread side, practically making the worker unusable.
///
/// ## Invariants
///
/// The slice inside this type was obtained by [riot_sys::suit_worker_try_prepare]. Once it is passed
/// on to [riot_sys::suit_worker_trigger_prepared], self is consumed.
///
/// This type is currently implemented in terms of a `&'static mut [u8]`. While the statement that
/// anyone with exclusive access to the ManifestLock also has exclusive access to the inner memory
/// is true, the lifetime is not truly static, and the implementation takes care to not exchange
/// the slice, or to hand it out for longer than the exclusive access to self.
pub struct ManifestLock(&'static mut [u8]);

impl ManifestLock {
    pub fn new(size: usize) -> Result<ManifestLock, ManifestLockError> {
        // unsafe: suit_worker_perpare is always safe
        let mut ptr = core::ptr::null_mut();
        let mut size_argument = size.try_into().unwrap_or(u32::MAX);
        let result = unsafe { riot_sys::suit_worker_try_prepare(&mut ptr, &mut size_argument) };
        result.negative_to_error()
            .map(|_| Self(unsafe { core::slice::from_raw_parts_mut(ptr, size) }))
            .map_err(|e| if e == ENOMEM {
                ManifestLockError::TooLarge(size_argument.try_into().unwrap_or(usize::MAX))
            } else {
                ManifestLockError::Busy
            })
    }

    /// Writing accessor to the locked slice
    pub fn as_mut(&mut self) -> &mut [u8] {
        &mut self.0
    }

    /// Start a SUIT worker thread based on the data that has been populated into the manifest. On
    /// typical valid manifests, that thread starts loading any images referred to in the manifest,
    /// and upgrades to them.
    ///
    /// Note that RIOT currently does not have any back channel -- this is a
    /// fire-and-hope-for-the-best call. (Some debug information is often sent to stdout).
    pub fn start_worker(self) {
        // unsafe: self was created according to suit_worker_trigger_prepared's documentation.
        unsafe { riot_sys::suit_worker_trigger_prepared(self.0.as_ptr(), self.0.len() as _) };
        // We've passed that lock on, therefore self does not uphold its invariants any more.
        core::mem::forget(self);
    }
}

impl Drop for ManifestLock {
    fn drop(&mut self) {
        // unsafe: This is the documented way to dispose of a lock that we turned out not to need.
        unsafe { riot_sys::suit_worker_trigger_prepared(self.0.as_ptr(), 0) };
    }
}

}

use suit::{ManifestLock, ManifestLockError};

mod block1 {

/// A server-side processor for Block1 transfers (requests composed of multiple blocks), without
/// consideration for Block2.
///
/// ## Extensibility
///
/// This only caters for one simultaneous transfer; any incoming Block1:0/m/s request invalidates prior
/// transfers. (More elaborate designs, eg. with multiple slots or one rejecting the Block1=0/m/s
/// while a transfer is still busy, would be possible, but are currently not implemented --
/// especially because they wouldn't easily drop all their overhead just because they're
/// parametrized to just one slot and a timeout of zero).
pub struct Block1Processing<T> {
    // FIXME: not pub
    pub buffer: T,
    // Output of some hash over the block options.
    //
    // The safety of that hash is not security relevant: While it allows an attacker to insert
    // blocks, SUIT manifests are authenticated later. (To satisfy the RFC9175 requirements, one'd
    // need a secretly seeded strong hash function).
    // FIXME: not pub
    pub block_hash: u64,
    // We're not keeping track of which blocks we received. We *could* store a bitfield based on
    // the bound we know (from configuration) to be on the ManifestLock area, but really why
    // bother, the process won't go through anyway.
}

}

use block1::Block1Processing;

#[derive(Default)]
struct ManifestPostPoint {
    status: Option<Block1Processing<ManifestLock>>,
}

impl coap_handler::Reporting for ManifestPostPoint {
    type Record<'a> = coap_handler_implementations::wkc::EmptyRecord where Self: 'a;
    type Reporter<'a> = core::iter::Once<coap_handler_implementations::wkc::EmptyRecord> where Self: 'a;
    fn report(&self) -> Self::Reporter<'_> {
        core::iter::once(coap_handler_implementations::wkc::EmptyRecord)
    }
}

enum BadSize1Detail {
    OptionTooLarge,
    Duplicate,
}

enum Response {
    MethodNotAllowed,
    Continue(coap_handler_implementations::helpers::Block1Data),
    InternalServerError,
    Unavailable,
    BadSize1(BadSize1Detail),
    Size1Missing,
    MissingFirstBlock,
    BadManifestLock(ManifestLockError),
    OutOfBounds,
    Changed(Option<coap_handler_implementations::helpers::Block1Data>),
}

use Response::*;

impl coap_handler::Handler for ManifestPostPoint {
    type RequestData = Result<Response, coap_handler_implementations::option_processing::BadOption>;
    fn extract_request_data(&mut self, req: &impl coap_message::ReadableMessage) -> Self::RequestData {
        if req.code().into() != coap_numbers::code::POST {
            return Ok(Response::MethodNotAllowed);
        }

        use coap_handler_implementations::option_processing::OptionsExt;
        use coap_message::MessageOption;

        let mut block1: Option<coap_handler_implementations::helpers::Block1Data> = None;
        let mut size1 = None;
        let mut hash = crc::Crc::<u64>::new(&crc::CRC_64_ECMA_182);
        let mut hash = hash.digest();
        req.options()
            .take_into(&mut block1)
            .filter(|o| {
                if o.number() == coap_numbers::option::SIZE1 {
                    if size1.is_some() {
                        size1 = Some(Err(BadSize1Detail::Duplicate));
                    } else {
                        // cast: We're implicitly assuming that u32 can be converted into usize.
                        // Both "this value can't" (a manifest larger than our address space) and
                        // "we're missing out" (a 4GB manifest on a 64bit system) are unrealistic
                        // enough and OKishly handled. (Especially as the size is u32 universally
                        // on the C side).
                        size1 = Some(o.value_uint::<u32>().ok_or(BadSize1Detail::OptionTooLarge).map(|s| s as _));
                    }
                    false
                } else {
                    true
                }
            })
            .map(|o| {
                hash.update(&o.number().to_le_bytes());
                hash.update(o.value());
                o
            })
            .ignore_elective_others()?;
        let block_hash = hash.finalize();

        let is_first = block1.as_ref().map(|b| b.start() == 0);
        let has_more = block1.as_ref().map(|b| b.more() == false);
        let size1 = match (size1, is_first, has_more) {
            (Some(Ok(s)), _, _) => s, // Size1 given, we'll trust it.
            (Some(Err(e)), _, _) => {
                // Actually bad Size1 option (too long for usize, or duplicate)
                return Ok(BadSize1(e));
            }
            (None, None, None) => req.payload().len(), // single-block message
            (None, Some(true), Some(false)) => req.payload().len(), // single-block message but
                                                                    // still block1 option present
            (None, Some(false), _) => 0, // FIXME: We don't care and don't need to know -- should
                                         // be expressed in types
            (None, _, _) => {
                // No size1 and non-trivial block1 option
                return Ok(Size1Missing);
            }
        };


        if block1.as_ref().map(|b| b.start()).unwrap_or_default() == 0 {
            // Block operation restarting: Drop the old status, thereby freeing the SUIT worker.
            self.status = None;
            let buffer = ManifestLock::new(size1);
            let buffer = match buffer {
                Ok(buffer) => buffer,
                Err(e) => {
                    return Ok(BadManifestLock(e));
                }
            };
            self.status = Some(Block1Processing {
                buffer,
                block_hash,
            });
        }
        // We're ignoring later changes to size1.

        let Some(status) = self.status.as_mut() else {
            return Ok(MissingFirstBlock);
        };
        if status.block_hash != block_hash {
            return Ok(MissingFirstBlock);
        }

        let start = block1.as_ref().map(|b| b.start()).unwrap_or(0);

        let Some(mut our_slice) = status.buffer.as_mut().get_mut(start..).and_then(|b| b.get_mut(..req.payload().len())) else {
            return Ok(OutOfBounds);
        };
        our_slice.copy_from_slice(req.payload());

        let block1 = match block1 {
            Some(block1) if block1.more() => {
                return Ok(Continue(block1));
            },
            block1 => block1,
        };

        // unwrap: Up to just now when we entered a scope where NLL said we can drop it, we
        // just had a &mut to its Some(value) in the local `status`.
        let status = self.status.take().unwrap();
        status.buffer.start_worker();
        println!("Starting SUIT process");

        Ok(Changed(block1))
    }
    fn estimate_length(&mut self, r: &Self::RequestData) -> usize {
        10 // enough even for rendered errors
    }
    fn build_response(&mut self, m: &mut impl coap_message::MutableWritableMessage, r: Self::RequestData) {
        let code = match r {
            Ok(MethodNotAllowed) => coap_numbers::code::METHOD_NOT_ALLOWED,
            Ok(InternalServerError) => coap_numbers::code::INTERNAL_SERVER_ERROR,
            Ok(Unavailable) => coap_numbers::code::SERVICE_UNAVAILABLE,
            Ok(BadSize1(_)) => coap_numbers::code::BAD_OPTION,
            Ok(Size1Missing) => coap_numbers::code::BAD_REQUEST, // FIXME details?
            Ok(MissingFirstBlock) => coap_numbers::code::REQUEST_ENTITY_INCOMPLETE,
            Ok(OutOfBounds) => coap_numbers::code::BAD_REQUEST, // FIXME details?
            Ok(BadManifestLock(ManifestLockError::Busy)) => coap_numbers::code::SERVICE_UNAVAILABLE,
            Ok(BadManifestLock(ManifestLockError::TooLarge(s))) => {
                m.set_code(coap_numbers::code::REQUEST_ENTITY_TOO_LARGE.try_into().ok().unwrap());
                m.add_option_uint(coap_numbers::option::SIZE1.try_into().ok().unwrap(), s as u32);
                return;
            }
            Ok(Changed(b1)) => {
                m.set_code(coap_numbers::code::CHANGED.try_into().ok().unwrap());
                if let Some(b1) = b1 {
                    m.add_option_uint(coap_numbers::option::BLOCK1.try_into().ok().unwrap(), b1.to_option_value());
                }
                return;
            }
            Ok(Continue(b1)) => {
                m.set_code(coap_numbers::code::CONTINUE.try_into().ok().unwrap());
                m.add_option_uint(coap_numbers::option::BLOCK1.try_into().ok().unwrap(), b1.to_option_value());
                return;
            }
            Err(e) => {
                use coap_handler_implementations::helpers::Renderable;
                e.render(m);
                return;
            }
        };
        m.set_code(code.try_into().ok().unwrap());
        m.set_payload(b"");
    }
}

struct RiotbootSlot;

impl coap_handler_implementations::SimpleRenderable for RiotbootSlot {
    fn render<W: core::fmt::Write>(&mut self, w: &mut W) {
        write!(w, "{}", unsafe { riot_sys::riotboot_slot_current() });
    }
}

struct RiotbootVersion;

impl coap_handler_implementations::SimpleRenderable for RiotbootVersion {
    fn render<W: core::fmt::Write>(&mut self, w: &mut W) {
        write!(w, "{}", unsafe { (*riot_sys::riotboot_slot_get_hdr(riot_sys::riotboot_slot_current() as _)).version });
    }
}

struct SuitIdFromFunction(unsafe extern "C" fn() -> *mut riot_sys::uuid_t);

struct Uuid([u8; 16]);

impl<C> minicbor::encode::Encode<C> for Uuid {
    fn encode<W: minicbor::encode::Write>(
        &self,
        e: &mut minicbor::encode::Encoder<W>,
        ctx: &mut C
    ) -> Result<(), minicbor::encode::Error<W::Error>> {
        e.tag(minicbor::data::Tag::Unassigned(37))?
            .bytes(&self.0)?;
        Ok(())
    }
}

impl coap_handler_implementations::TypeRenderable for SuitIdFromFunction {
    // We could use a UUID type, but given we neither parse them nor produce them, that seems
    // silly.
    type Get = Uuid;
    type Post = ();
    type Put = ();

    fn get(&mut self) -> Result<Self::Get, u8> {
        // is it safe to read without further synchronization?
        let id = unsafe { (self.0)().read() };
        // that's weird but I think from the C casts that's how it's intended to be used, could
        // have a union.
        Ok(Uuid(unsafe { core::mem::transmute(id) }))
    }
}

fn main() {
    extern "C" {
        fn do_vfs_init();
    }

    unsafe { do_vfs_init() };

    let handler = coap_message_demos::full_application_tree(None)
        .below(&["ps"], riot_coap_handler_demos::ps::ps_tree())
        .below(&["vfs"], riot_coap_handler_demos::vfs::vfs("/const"))
        .at(&["suit"], ManifestPostPoint::default())

        .at(&["riotboot", "current", "slot"], coap_handler_implementations::SimpleRendered(RiotbootSlot))
        .at(&["riotboot", "current", "version"], coap_handler_implementations::SimpleRendered(RiotbootVersion))
        .at_with_attributes(&["suit", "cond", "vendor-id"], &[], coap_handler_implementations::TypeHandler::new_minicbor(SuitIdFromFunction(riot_sys::suit_get_vendor_id as _)))
        .at_with_attributes(&["suit", "cond", "class-id"], &[], coap_handler_implementations::TypeHandler::new_minicbor(SuitIdFromFunction(riot_sys::suit_get_class_id as _)))
        .at_with_attributes(&["suit", "cond", "device-id"], &[], coap_handler_implementations::TypeHandler::new_minicbor(SuitIdFromFunction(riot_sys::suit_get_device_id as _)))
        .with_wkc()
        ;
    let mut handler = riot_wrappers::coap_handler::GcoapHandler(handler);

    let mut listener = gcoap::SingleHandlerListener::new_catch_all(&mut handler);

    gcoap::scope(|greg| {
        greg.register(&mut listener);

        println!("CoAP server ready; waiting for interfaces to settle before reporting addresses...");

        let sectimer = ztimer::Clock::sec();
        sectimer.sleep_ticks(2);

        for netif in gnrc::Netif::all() {
            println!("Active interface from PID {:?} ({:?})", netif.pid(), netif.pid().get_name().unwrap_or("unnamed"));
            match netif.ipv6_addrs() {
                Ok(addrs) => {
                    for a in &addrs {
                        println!("    Address {:?}", a);
                    }
                }
                _ => {
                    println!("    Does not support IPv6.");
                }
            }
        }

        // Sending main thread to sleep; can't return or the Gcoap handler would need to be
        // deregistered (which it can't).
        loop { thread::sleep(); }
    })
}
