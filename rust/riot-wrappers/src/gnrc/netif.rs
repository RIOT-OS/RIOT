//! Components acting on the netif pktsnip layer
#![cfg(riot_module_gnrc_pktbuf)]

// FIXME: Move some of mod.rs in here

use crate::gnrc_pktbuf::{Mode, NotEnoughSpace, Pktsnip, Writable};
use crate::thread::KernelPID;
use riot_sys::{gnrc_netif_hdr_t, gnrc_nettype_t_GNRC_NETTYPE_NETIF as GNRC_NETTYPE_NETIF};

/// A transparent wrapper around ``gnrc_netif_hdr_t`` that provides idiomatically typed fields
#[doc(alias = "gnrc_netif_hdr_t")]
#[derive(Copy, Clone)]
pub struct Header<'a>(&'a gnrc_netif_hdr_t);

impl Header<'_> {
    pub fn if_pid(&self) -> Option<KernelPID> {
        KernelPID::new(self.0.if_pid)
    }
}

/// The root of building a Netif pktsnip
pub struct HeaderBuilder<M: Mode>(Pktsnip<M>);

impl<M: Mode> HeaderBuilder<M> {
    #[inline]
    pub fn without_link_layer_addresses(self) -> HeaderBuilt {
        self.with_src_and_dst(&[], &[])
    }

    #[inline]
    pub fn with_src(self, src: &[u8]) -> HeaderBuilt {
        self.with_src_and_dst(src, &[])
    }

    #[inline]
    pub fn with_dst(self, dst: &[u8]) -> HeaderBuilt {
        self.with_src_and_dst(&[], dst)
    }

    pub fn with_src_and_dst(self, src: &[u8], dst: &[u8]) -> HeaderBuilt {
        let Ok(src_len) = src.len().try_into() else {
            return HeaderBuilt(Err(NotEnoughSpace));
        };
        let Ok(dst_len) = dst.len().try_into() else {
            return HeaderBuilt(Err(NotEnoughSpace));
        };
        // "as *mut": I'm assuming the C signature is just missing its const
        let snip = unsafe {
            riot_sys::gnrc_netif_hdr_build(
                src.as_ptr() as *mut _,
                src_len,
                dst.as_ptr() as *mut _,
                dst_len,
            )
        };
        if snip == 0 as *mut _ {
            return HeaderBuilt(Err(NotEnoughSpace));
        } else {
            // unsafe: snip is initialized, and to_ptr takes care of the refcounts
            // FIXME can use add?
            unsafe { (*snip).next = self.0.to_ptr() };
            HeaderBuilt(Ok(unsafe { Pktsnip::<Writable>::from_ptr(snip) }))
        }
    }
}

pub struct HeaderBuilt(Result<Pktsnip<Writable>, NotEnoughSpace>);

impl HeaderBuilt {
    pub fn finish(self) -> Result<Pktsnip<Writable>, NotEnoughSpace> {
        self.0
    }

    fn access(&mut self) -> Option<&mut riot_sys::gnrc_netif_hdr_t> {
        match &mut self.0 {
            Ok(snip) => Some(
                // Unsafe: Valid by construction of our type
                unsafe {
                    (snip.data_mut().as_mut_ptr() as *mut riot_sys::gnrc_netif_hdr_t).as_mut()
                }
                .expect("Non-null by construction"),
            ),
            Err(_) => None,
        }
    }

    pub fn with_if_pid(mut self, pid: KernelPID) -> HeaderBuilt {
        if let Some(h) = self.access() {
            h.if_pid = pid.into();
        }
        self
    }
}

impl<M: Mode> Pktsnip<M> {
    /// Get the Netif header of the snip, if there is any thusly typed snip present
    // Note that we can *not* just implement this with &mut on a Writable Pktsnip, because
    // writability is only ever about the first snip
    pub fn netif_get_header(&self) -> Option<Header<'_>> {
        let netif_snip = self.search_type(GNRC_NETTYPE_NETIF)?;
        // unsafe: Following GNRC conventions
        // unwrap: pointer comes from as_ptr and is thus non-null (but doesn't tell that)
        let header =
            unsafe { (netif_snip.data.as_ptr() as *const gnrc_netif_hdr_t).as_ref() }.unwrap();
        // unsafe: Using C API as documented
        debug_assert!(
            netif_snip.data.len()
                == unsafe { riot_sys::inline::gnrc_netif_hdr_sizeof(crate::inline_cast(header)) }
                    as _
        );
        Some(Header(header))
    }

    /// Build a netif header around the Pktsnip
    #[doc(alias = "gnrc_netif_hdr_build")]
    pub fn netif_hdr_builder(self) -> HeaderBuilder<M> {
        HeaderBuilder(self)
    }
}
