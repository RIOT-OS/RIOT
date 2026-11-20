use super::{KernelPID, Status};

use crate::libc;
use core::ffi::CStr;
use core::marker::PhantomData;
use core::mem::transmute;
use riot_sys as raw;

/// Internal helper that does all the casting but relies on the caller to establish appropriate
/// lifetimes.
///
/// This also returns a pointer to the created thread's control block inside the stack; that TCB
/// can be used to get the thread's status even when the thread is already stopped and the PID may
/// have been reused for a different thread. For short-lived threads that are done before this
/// function returns, the TCB may be None.
unsafe fn create<R>(
    stack: &mut [u8],
    closure: &mut R,
    name: &CStr,
    priority: u8,
    flags: i32,
) -> (raw::kernel_pid_t, Option<*mut riot_sys::thread_t>)
where
    R: Send + FnMut(),
{
    // overwriting name "R" as suggested as "copy[ing] over the parameters" on
    // https://doc.rust-lang.org/error-index.html#E0401
    unsafe extern "C" fn run<R>(x: *mut libc::c_void) -> *mut libc::c_void
    where
        R: Send + FnMut(),
    {
        let closure: &mut R = transmute(x);
        closure();
        0 as *mut libc::c_void
    }

    let pid = raw::thread_create(
        transmute(stack.as_mut_ptr()),
        stack.len() as i32,
        priority,
        flags,
        Some(run::<R>),
        closure as *mut R as *mut _,
        name.as_ptr() as _,
    );

    let tcb = riot_sys::thread_get(pid);
    // FIXME: Rather than doing pointer comparisons, it'd be nicer to just get the stack's
    // calculated thread control block (TCB) position and look right in there.
    let tcb = if tcb >= &stack[0] as *const u8 as *mut _
        && tcb <= &stack[stack.len() - 1] as *const u8 as *mut _
    {
        Some(crate::inline_cast_mut(tcb))
    } else {
        None
    };

    (pid, tcb)
}

/// Create a context for starting threads that take shorter than 'static references.
///
/// Inside the scope, threads can be created using the `.spawn()` method of the scope passed in,
/// similar to the scoped-threads RFC (which resembles crossbeam's threads). Unlike that, the scope
/// has no dynamic memory of the spawned threads, and no actual way of waiting for a thread. If the
/// callback returns, the caller has call the scope's `.reap()` method with all the threads that
/// were launched; otherwise, the program panics.
pub fn scope<'env, F, R>(callback: F) -> R
where
    F: for<'id> FnOnce(&mut CountingThreadScope<'env, 'id>) -> R,
{
    let mut s = CountingThreadScope {
        threads: 0,
        _phantom: PhantomData,
    };

    let ret = callback(&mut s);

    s.wait_for_all();

    ret
}

/// Lifetimed helper through which threads can be spawned.
///
/// ## Lifetimes
///
/// The involved lifetimes ensure that all parts used to build the thread (its closure, stack, and
/// name) outlive the whole process, which (given the generally dynamic lifetime of threads) can
/// only be checked dynamically.
///
/// The lifetimes are:
///
/// * `'env`: A time surrounding the [`scope()`] call. All inputs to the thread are checked to live
///   at least that long (possibly longer; it is commonplace for them to be `'static`).
/// * `'id`: An identifying lifetime (or brand) of the scope. Its lifetime is somewhere inbetween
///   the outer `'env` and the run time of the called closure.
///
///   Practically, don't think of this as a lifetime, but more as a disambiguator: It makes the
///   monomorphized CountingThreadScope unique in the sense that no two instances of
///   CountingThreadScope can ever have the same type.
///
///   By having unique types, it is ensured that a counted thread is only counted down (in
///   [`.reap()`]) in the scope it was born in, and that no shenanigans with counters being swapped
///   around with [core::mem::swap()] are used to trick the compiler into allowing use-after-free.
///
/// This technique was inspired by (and is explained well) in [the GhostCell
/// Paper](http://plv.mpi-sws.org/rustbelt/ghostcell/paper.pdf).
///
pub struct CountingThreadScope<'env, 'id> {
    threads: u16, // a counter, but larger than kernel_pid_t
    _phantom: PhantomData<(&'env (), &'id ())>,
}

impl<'env, 'id> CountingThreadScope<'env, 'id> {
    /// Start a thread in the given stack, in which the closure is run. The thread gets a human
    /// readable name (ignored in no-DEVHELP mode), and is started with the priority and flags as
    /// per thread_create documentation.
    ///
    /// The returned thread object can safely be discarded when the scope is not expected to ever
    /// return, and needs to be passed on to `.reap()` otherwise.
    ///
    /// Having the closure as a mutable reference (rather than a moved instance) is a bit
    /// unergonomic as it means that `spawn(..., || { foo }, ..)` one-line invocations are
    /// impossible, but is necessary as it avoids having the callback sitting in the Thread which
    /// can't be prevented from moving around on the stack between the point when thread_create is
    /// called (and the pointer is passed on to RIOT) and the point when the threads starts running
    /// and that pointer is used.
    pub fn spawn<R>(
        &mut self,
        stack: &'env mut [u8],
        closure: &'env mut R,
        name: &'env CStr,
        priority: u8,
        flags: i32,
    ) -> Result<CountedThread<'id>, raw::kernel_pid_t>
    where
        R: Send + FnMut(),
    {
        self.threads = self.threads.checked_add(1).expect("Thread limit exceeded");

        let (pid, tcb) = unsafe { create(stack, closure, name, priority, flags) };

        if pid < 0 {
            return Err(pid);
        }

        Ok(CountedThread {
            thread: TrackedThread {
                pid: KernelPID(pid),
                tcb: tcb,
            },
            _phantom: PhantomData,
        })
    }

    /// Assert that the thread has terminated, and remove it from the list of pending threads in
    /// this context.
    ///
    /// Unlike a (POSIX) wait, this will not block (for there is no SIGCHLDish thing in RIOT --
    /// whoever wants to be notified would need to make their threads send an explicit signal), but
    /// panic if the thread is not actually done yet.
    pub fn reap(&mut self, thread: CountedThread<'id>) {
        match thread.status() {
            Status::Stopped => (),
            _ => panic!("Attempted to reap running process"),
        }

        self.threads -= 1;
    }

    fn wait_for_all(self) {
        if self.threads != 0 {
            panic!("Not all threads were waited for at scope end");
        }
    }
}

// The 'id ensures that threads can only be reaped where they were created. (It might make sense to
// move it into TrackedThread and make the tcb usable for more than just pointer comparison).
#[derive(Debug)]
pub struct CountedThread<'id> {
    thread: TrackedThread,
    _phantom: PhantomData<&'id ()>,
}

impl<'id> CountedThread<'id> {
    pub fn pid(&self) -> KernelPID {
        self.thread.pid()
    }

    pub fn status(&self) -> Status {
        self.thread.status()
    }
}

/// Create a thread with a statically allocated stack
pub fn spawn<R>(
    stack: &'static mut [u8],
    closure: &'static mut R,
    name: &'static CStr,
    priority: u8,
    flags: i32,
) -> Result<TrackedThread, raw::kernel_pid_t>
where
    R: Send + FnMut(),
{
    let (pid, tcb) = unsafe { create(stack, closure, name, priority, flags) };

    if pid < 0 {
        return Err(pid);
    }

    Ok(TrackedThread {
        pid: KernelPID(pid),
        tcb,
    })
}

/// A thread identified not only by its PID (which can be reused whenever the thread has quit) but
/// also by a pointer to its thread control block. This gives a TrackedThread a better get_status()
/// method that reliably reports Stopped even when the PID is reused.
///
/// A later implementation may stop actually having the pid in the struct and purely rely on the
/// tcb (although that'll need to become a lifetime'd reference to a cell by then).
#[derive(Debug)]
pub struct TrackedThread {
    pid: KernelPID,
    // If this is None, then the thread was so short-lived that the TCB couldn't even be extracted
    tcb: Option<*mut riot_sys::thread_t>,
}

impl TrackedThread {
    pub fn pid(&self) -> KernelPID {
        self.pid
    }

    /// Like status of a KernelPID, but infallible: this returns Stopped if the PID has been
    /// re-used after our thread has stopped.
    // FIXME: This can probably be simplified a lot by just looking into the TCB if it were
    // obtained reliably
    pub fn status(&self) -> Status {
        let status = self.pid.status();
        let tcb = self.pid.thread();
        if let (Ok(status), Ok(tcb), Some(startup_tcb)) = (status, tcb, self.tcb) {
            if crate::inline_cast(tcb) == startup_tcb {
                status
            } else {
                Status::Stopped
            }
        } else {
            // Thread not in task list, so it's obviousy stopped
            Status::Stopped
        }
    }
}
