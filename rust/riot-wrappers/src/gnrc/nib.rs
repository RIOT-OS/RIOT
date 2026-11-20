/// A single entry in the neighbor cache.
///
/// These can be obtained by iterating
pub struct NcEntry(riot_sys::gnrc_ipv6_nib_nc_t);

/// Neighbor Unreachability Detection state
///
/// See
/// [the nib_nc documentation](https://doc.riot-os.org/group__net__gnrc__ipv6__nib__nc.html)
/// for more detailed semantics
// FIXME can we pull doc from riot_sys?
#[derive(Debug)]
pub enum NudState {
    Unmanaged,
    Unreachable,
    Incomplete,
    Stale,
    Delay,
    Probe,
    Reachable,
}

impl NudState {
    fn from_c(input: crate::libc::c_uint) -> Option<Self> {
        Some(match input {
            riot_sys::GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED => NudState::Unmanaged,
            riot_sys::GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE => NudState::Unreachable,
            riot_sys::GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE => NudState::Incomplete,
            riot_sys::GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE => NudState::Stale,
            riot_sys::GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY => NudState::Delay,
            riot_sys::GNRC_IPV6_NIB_NC_INFO_NUD_STATE_PROBE => NudState::Probe,
            riot_sys::GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE => NudState::Reachable,
            _ => return None,
        })
    }

    /// Returns a plain text label of the state.
    ///
    /// This is equivalent to debug (except for capitalization), but more versatile in its use due
    /// to its type.
    pub fn label(&self) -> &'static str {
        match self {
            NudState::Unmanaged => "managed",
            NudState::Unreachable => "unreachable",
            NudState::Incomplete => "incomplete",
            NudState::Stale => "stale",
            NudState::Delay => "delay",
            NudState::Probe => "probe",
            NudState::Reachable => "reachable",
        }
    }
}

/// 6LoWPAN address registration (6Lo-AR) state
///
/// See
/// [the nib_nc documentation](https://doc.riot-os.org/group__net__gnrc__ipv6__nib__nc.html)
/// for more detailed semantics
// FIXME can we pull doc from riot_sys?
#[derive(Debug)]
pub enum ArState {
    Gc,
    Tentative,
    Registered,
    Manual,
}

impl ArState {
    fn from_c(input: crate::libc::c_uint) -> Option<Self> {
        Some(match input {
            riot_sys::GNRC_IPV6_NIB_NC_INFO_AR_STATE_GC => ArState::Gc,
            riot_sys::GNRC_IPV6_NIB_NC_INFO_AR_STATE_TENTATIVE => ArState::Tentative,
            riot_sys::GNRC_IPV6_NIB_NC_INFO_AR_STATE_REGISTERED => ArState::Registered,
            riot_sys::GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL => ArState::Manual,
            _ => return None,
        })
    }

    /// Returns a plain text label of the state.
    ///
    /// This is equivalent to debug (except for capitalization), but more versatile in its use due
    /// to its type.
    pub fn label(&self) -> &'static str {
        match self {
            ArState::Gc => "GC",
            ArState::Tentative => "tentative",
            ArState::Registered => "registered",
            ArState::Manual => "manual",
        }
    }
}

impl NcEntry {
    /// Iterate over the Neighbor Cache.
    #[doc(alias = "gnrc_ipv6_nib_nc_iter")]
    pub fn all() -> impl Iterator<Item = Self> {
        // If we add anything like all_nc_entries_on_interface():
        // // Interfaces are positive numbers; MAX is clearly out of range and allows us to have an easier
        // // input type
        // let interface = interface.map(|i| {
        //     crate::libc::c_uint::try_from(usize::from(i)).unwrap_or(crate::libc::c_uint::MAX)
        // });

        any_nc_query(0)
    }

    pub fn l2addr(&self) -> &[u8] {
        &self.0.l2addr[..self.0.l2addr_len as usize]
    }

    pub fn ipv6_addr(&self) -> &crate::gnrc::ipv6::Address {
        // unsafe: It's repr(transparent) around it
        unsafe { core::mem::transmute(&self.0.ipv6) }
    }

    #[doc(alias = "gnrc_ipv6_nib_nc_get_iface")]
    pub fn iface(&self) -> Option<core::num::NonZero<usize>> {
        const {
            assert!(riot_sys::KERNEL_PID_UNDEF == 0, "Interface lookup mixes unspecified interface with PIDs and thus relies on the unspecified latter being 0.")
        };
        let interface = unsafe {
            riot_sys::inline::gnrc_ipv6_nib_nc_get_iface(crate::inline_cast_ref(&self.0))
        };
        // Let's not get into size discussions
        let interface = interface as usize;
        interface.try_into().ok()
    }

    #[doc(alias = "gnrc_ipv6_nib_nc_is_router")]
    pub fn is_router(&self) -> bool {
        unsafe { riot_sys::inline::gnrc_ipv6_nib_nc_is_router(crate::inline_cast_ref(&self.0)) }
    }

    /// Access the entry's Neighbor Unreachability Detection (NUD) state
    ///
    /// This is None if the interface's NUD state is invalid (including values introduced to RIOT
    /// OS but not known to riot-wrappers).
    pub fn nud_state(&self) -> Option<NudState> {
        let result = NudState::from_c(unsafe {
            riot_sys::inline::gnrc_ipv6_nib_nc_get_nud_state(crate::inline_cast_ref(&self.0))
        });
        result
    }

    /// Access the entry's 6LoWPAN address registration (6Lo-AR) state
    ///
    /// This is None if the interface's neighbor  state is invalid (including values
    /// introduced to RIOT OS but not known to riot-wrappers).
    pub fn ar_state(&self) -> Option<ArState> {
        let result = ArState::from_c(unsafe {
            riot_sys::inline::gnrc_ipv6_nib_nc_get_ar_state(crate::inline_cast_ref(&self.0))
        });
        result
    }
}

struct NcIterator {
    interface: crate::libc::c_uint,
    state: *mut crate::libc::c_void,
}

impl Iterator for NcIterator {
    type Item = NcEntry;

    fn next(&mut self) -> Option<Self::Item> {
        let mut nc_entry = core::mem::MaybeUninit::<riot_sys::gnrc_ipv6_nib_nc_t>::uninit();
        if unsafe {
            riot_sys::gnrc_ipv6_nib_nc_iter(self.interface, &mut self.state, nc_entry.as_mut_ptr())
        } {
            let nc_entry = NcEntry(unsafe { nc_entry.assume_init() });
            Some(nc_entry)
        } else {
            None
        }
    }
}

fn any_nc_query(interface: crate::libc::c_uint) -> impl Iterator<Item = NcEntry> {
    NcIterator {
        interface,
        state: core::ptr::null_mut(),
    }
}
