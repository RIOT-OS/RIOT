//! RIOT (C) thread implementation
use riot_sys as raw;

use super::{NoSuchThread, StackStats, StackStatsError};
use crate::helpers::PointerToCStr;

/// Offloaded tools for creation
mod creation;
pub use creation::{scope, spawn, CountedThread, CountingThreadScope};

/// Wrapper around a valid (not necessarily running, but in-range) [riot_sys::kernel_pid_t] that
/// provides access to thread details and signaling.
// Possible optimization: Make this NonZero (especially now that there are const checks for this in
// the NonZero conversion anyway)
#[derive(Debug, PartialEq, Copy, Clone)]
pub struct KernelPID(pub(crate) raw::kernel_pid_t);

// Converting the raw constants into consistently typed ones

// pub(crate) const KERNEL_PID_UNDEF: riot_sys::kernel_pid_t = riot_sys::KERNEL_PID_UNDEF as _;
const KERNEL_PID_FIRST: riot_sys::kernel_pid_t = riot_sys::KERNEL_PID_FIRST as _;
const KERNEL_PID_LAST: riot_sys::kernel_pid_t = riot_sys::KERNEL_PID_LAST as _;
pub(crate) const KERNEL_PID_ISR: riot_sys::kernel_pid_t = riot_sys::KERNEL_PID_ISR as _;

// Converting the raw constants into consistently typed ones for use in match branches. If
// that becomes a pattern, it might make sense to introduce a macro that forces a bunch of
// symbols (with different capitalizations) into a given type and makes an enum with a
// from_int method out of it.

// This is special because it is not part of the enum but a cast -1
// unsafe: Side effect free C macros
const STATUS_NOT_FOUND: i32 = unsafe { riot_sys::macro_STATUS_NOT_FOUND() as _ };

const STATUS_STOPPED: i32 = riot_sys::thread_status_t_STATUS_STOPPED as i32;
const STATUS_SLEEPING: i32 = riot_sys::thread_status_t_STATUS_SLEEPING as i32;
const STATUS_MUTEX_BLOCKED: i32 = riot_sys::thread_status_t_STATUS_MUTEX_BLOCKED as i32;
const STATUS_RECEIVE_BLOCKED: i32 = riot_sys::thread_status_t_STATUS_RECEIVE_BLOCKED as i32;
const STATUS_SEND_BLOCKED: i32 = riot_sys::thread_status_t_STATUS_SEND_BLOCKED as i32;
const STATUS_REPLY_BLOCKED: i32 = riot_sys::thread_status_t_STATUS_REPLY_BLOCKED as i32;
const STATUS_FLAG_BLOCKED_ANY: i32 = riot_sys::thread_status_t_STATUS_FLAG_BLOCKED_ANY as i32;
const STATUS_FLAG_BLOCKED_ALL: i32 = riot_sys::thread_status_t_STATUS_FLAG_BLOCKED_ALL as i32;
const STATUS_MBOX_BLOCKED: i32 = riot_sys::thread_status_t_STATUS_MBOX_BLOCKED as i32;
const STATUS_RUNNING: i32 = riot_sys::thread_status_t_STATUS_RUNNING as i32;
const STATUS_PENDING: i32 = riot_sys::thread_status_t_STATUS_PENDING as i32;

#[derive(Debug)]
#[non_exhaustive]
pub enum Status {
    // I would not rely on any properties of the assigned values, but it might make the conversion
    // points easier on the generated code if it can be reasoned down to a simple check of whether
    // it's in range.
    Stopped = STATUS_STOPPED as isize,
    Sleeping = STATUS_SLEEPING as isize,
    MutexBlocked = STATUS_MUTEX_BLOCKED as isize,
    ReceiveBlocked = STATUS_RECEIVE_BLOCKED as isize,
    SendBlocked = STATUS_SEND_BLOCKED as isize,
    ReplyBlocked = STATUS_REPLY_BLOCKED as isize,
    FlagBlockedAny = STATUS_FLAG_BLOCKED_ANY as isize,
    FlagBlockedAll = STATUS_FLAG_BLOCKED_ALL as isize,
    MboxBlocked = STATUS_MBOX_BLOCKED as isize,
    Running = STATUS_RUNNING as isize,
    Pending = STATUS_PENDING as isize,

    /// A status value not known to riot-wrappers. Don't match for this explicitly: Other values
    /// may, at any minor riot-wrappers update, become actual process states again.
    Other, // Not making this Other(i32) as by the time this is reached, the code can't react
           // meaningfully to it, and if that shows up in any debug output, someone will need to
           // reproduce this anyway and can hook into from_int then.
}

impl Status {
    fn from_int(status: i32) -> Self {
        match status {
            STATUS_STOPPED => Status::Stopped,
            STATUS_SLEEPING => Status::Sleeping,
            STATUS_MUTEX_BLOCKED => Status::MutexBlocked,
            STATUS_RECEIVE_BLOCKED => Status::ReceiveBlocked,
            STATUS_SEND_BLOCKED => Status::SendBlocked,
            STATUS_REPLY_BLOCKED => Status::ReplyBlocked,
            STATUS_FLAG_BLOCKED_ANY => Status::FlagBlockedAny,
            STATUS_FLAG_BLOCKED_ALL => Status::FlagBlockedAll,
            STATUS_MBOX_BLOCKED => Status::MboxBlocked,
            STATUS_RUNNING => Status::Running,
            STATUS_PENDING => Status::Pending,
            _ => Status::Other,
        }
    }
}

impl KernelPID {
    pub fn new(pid: raw::kernel_pid_t) -> Option<Self> {
        if unsafe { raw::pid_is_valid(pid) } != 0 {
            Some(KernelPID(pid))
        } else {
            None
        }
    }

    pub fn all_pids() -> impl Iterator<Item = KernelPID> {
        // Not constructing the KernelPID manually but going through new serves as a convenient
        // validation of the construction (all_pids will panic if the rules of pid_is_valid change,
        // and then this function *should* be reevaluated). As pid_is_valid is static inline, the
        // compiler should be able to see through the calls down to there that the bounds checked
        // for there are the very bounds used in the construction here.
        (KERNEL_PID_FIRST..=KERNEL_PID_LAST)
            .map(|i| KernelPID::new(i).expect("Should be valid by construction"))
    }

    pub fn get_name(&self) -> Option<&str> {
        // Shortcut through an otherwise unoptimizable function
        if !cfg!(riot_develhelp) {
            return None;
        }

        let ptr = unsafe { raw::thread_getname(self.0) };

        // If the thread stops, the name might be not valid any more, but then again the getname
        // function might already have returned anything, and thread names are generally strings in
        // .text. Unwrapping because by the time non-ASCII text shows up in there, something
        // probably already went terribly wrong.
        unsafe { ptr.to_lifetimed_cstr()? }.to_str().ok()
    }

    /// Get the current status of the thread of that number, if one currently exists
    #[doc(alias = "thread_getstatus")]
    pub fn status(&self) -> Result<Status, NoSuchThread> {
        // unsafe: Side effect free, always-callable C function
        let status = unsafe { raw::thread_getstatus(self.0) } as _;
        if status == STATUS_NOT_FOUND {
            Err(NoSuchThread)
        } else {
            Ok(Status::from_int(status))
        }
    }

    #[doc(alias = "thread_wakeup")]
    pub fn wakeup(&self) -> Result<(), NoSuchThread> {
        let success = unsafe { raw::thread_wakeup(self.0) };
        match success {
            1 => Ok(()),
            _ => Err(NoSuchThread),
        }
    }

    /// Pick the thread_t out of sched_threads for the PID
    #[doc(alias = "thread_get")]
    fn thread(&self) -> Result<*const riot_sys::thread_t, NoSuchThread> {
        // unsafe: C function's "checked" precondition met by type constraint on PID validity
        let t = unsafe { riot_sys::thread_get_unchecked(self.0) };
        // .as_ref() would have the null check built in, but we can't build a shared reference out
        // of this, only ever access its fields with volatility.
        if t == 0 as *mut _ {
            Err(NoSuchThread)
        } else {
            Ok(crate::inline_cast(t))
        }
    }

    pub fn priority(&self) -> Result<u8, NoSuchThread> {
        let thread = self.thread()?;
        Ok(unsafe { (*thread).priority })
    }

    /// Gather information about the stack's thread.
    ///
    /// A None being returned can have two reasons:
    /// * The thread does not exist, or
    /// * develhelp is not active.
    ///
    /// This is not backed by C functions (as most of the rest of this crate is), but rather a
    /// practical way to access struct members that may or may not be present in a build.
    pub fn stack_stats(&self) -> Result<StackStats, StackStatsError> {
        #[cfg(riot_develhelp)]
        {
            /// Out of a thread pointer, build whatever thread_measure_stack_free happens to need
            /// right now
            struct ThreadOrItsStart(*const riot_sys::thread_t);

            // Before https://github.com/RIOT-OS/RIOT/pull/18942 (up and including 2024.04)
            impl Into<*const crate::libc::c_char> for ThreadOrItsStart {
                fn into(self) -> *const crate::libc::c_char {
                    unsafe { (*self.0).stack_start }
                }
            }
            // After https://github.com/RIOT-OS/RIOT/pull/18942 (presumably 2024.07 and later)
            impl Into<*const riot_sys::inline::thread_t> for ThreadOrItsStart {
                fn into(self) -> *const riot_sys::inline::thread_t {
                    crate::inline_cast(self.0)
                }
            }

            let thread = self.thread()?;
            return Ok(StackStats {
                // This cast is relevant because different platforms (eg. native and arm) disagree on
                // whether that's an i8 or u8 pointer. Could have made it c_char, but a) don't want to
                // alter the signatures and b) it's easier to use on the Rust side with a clear type.
                start: unsafe { (*thread).stack_start as _ },
                size: unsafe { (*thread).stack_size as _ },
                #[allow(unused_imports)]
                free: unsafe {
                    use riot_sys::inline::*;
                    use riot_sys::*;
                    thread_measure_stack_free(ThreadOrItsStart(thread).into())
                } as usize,
            });
        }
        #[cfg(not(riot_develhelp))]
        return Err(StackStatsError::InformationUnavailable);
    }
}

impl Into<raw::kernel_pid_t> for &KernelPID {
    fn into(self) -> raw::kernel_pid_t {
        self.0
    }
}

impl Into<raw::kernel_pid_t> for KernelPID {
    fn into(self) -> raw::kernel_pid_t {
        self.0
    }
}

impl From<KernelPID> for core::num::NonZero<u16> {
    fn from(pid: KernelPID) -> Self {
        const { assert!(KERNEL_PID_FIRST > 0) };
        const { assert!(KERNEL_PID_LAST > 0) };
        const { assert!(core::mem::size_of::<riot_sys::kernel_pid_t>() == core::mem::size_of::<u16>()) };
        // unsafe: self.0 is checked to be within first..=last
        unsafe { core::num::NonZero::new_unchecked(pid.0 as u16) }
    }
}

/// PID of the currently active thread
#[doc(alias = "thread_getpid")]
pub fn get_pid() -> KernelPID {
    // Ignoring the volatile in thread_getpid because it's probably not necessary (any application
    // will only ever see a consistent current PID).
    KernelPID(unsafe { raw::thread_getpid() })
}

/// Put the current thread in the "sleeping" state, only to be continue when something calls
/// [KernelPID::wakeup()] on its PID.
#[doc(alias = "thread_sleep")]
pub fn sleep() {
    unsafe { raw::thread_sleep() }
}
