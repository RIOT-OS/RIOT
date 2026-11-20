//! Data-carrying mutex built using RIOT's [mutex] module
//!
//! This roughly mimics [std::sync::Mutex].
//!
//! [mutex]: https://doc.riot-os.org/group__core__sync__mutex.html
//! [std::sync::mutex]: https://doc.rust-lang.org/std/sync/struct.Mutex.html

use core::ops::{Deref, DerefMut};
// For correctness considerations, all uses of UnsafeCell can be ignored here; the only reason why
// an UnsafeCell is used is to indicate to the linker that a static mutex still needs to be
// allocated in .data and not in .text. (In other words: This is what allows transmuting the & to
// the inner data into a &mut).
use core::cell::UnsafeCell;

/// A mutual exclusion primitive useful for protecting shared data
///
/// Unlike the [std::sync::Mutex], this has no concept of poisoning, so waiting for mutexes in
/// panicked (and thus locked) threads will lock the accessing thread as well. This is because RIOT
/// threads don't unwind Rust code. As a consequence, the mutex interface is different from the
/// standard library's.
///
/// Several methods (into_inner, get_mut) are not implemented until they're actually needed.
///
/// [std::sync::mutex]: https://doc.rust-lang.org/std/sync/struct.Mutex.html
pub struct Mutex<T> {
    mutex: UnsafeCell<riot_sys::inline::mutex_t>,
    data: UnsafeCell<T>,
}

impl<T> Mutex<T> {
    /// Create a new mutex in an unlocked state
    #[doc(alias = "mutex_init")]
    pub const fn new(t: T) -> Mutex<T> {
        // unsafe: Side effect free C macro
        let new = unsafe { riot_sys::macro_MUTEX_INIT() };
        Mutex {
            data: UnsafeCell::new(t),
            mutex: UnsafeCell::new(new),
        }
    }

    /// Get an accessor to the mutex when the mutex is available
    ///
    /// ## Panics
    ///
    /// This function checks at runtime whether it is called in a thread context, and panics
    /// otherwise. Consider promoting a reference with an [`InThread`](crate::thread::InThread)
    /// token's [`.promote(&my_mutex)`](crate::thread::InThread::promote) to gain access to a
    /// better [`.lock()`](crate::thread::ValueInThread<&Mutex<T>>::lock) method, which suffers
    /// neither the panic condition nor the runtime overhead.
    #[doc(alias = "mutex_lock")]
    pub fn lock(&self) -> MutexGuard<'_, T> {
        crate::thread::InThread::new()
            .expect("Mutex::lock may only be called outside of interrupt contexts")
            .promote(self)
            .lock()
    }

    /// Get an accessor to the mutex if the mutex is available
    #[doc(alias = "mutex_trylock")]
    pub fn try_lock(&self) -> Option<MutexGuard<'_, T>> {
        match unsafe { riot_sys::mutex_trylock(self.mutex.get()) } {
            1 => Some(MutexGuard { mutex: &self }),
            _ => None,
        }
    }

    /// Lock the mutex and throw away the key
    ///
    /// Try to lock the mutex (returning None if it is locked). When successful, a mutable
    /// reference for the complete lifetime of the mutex is produced, without the usual mechanisms
    /// that'd free the mutex later.
    ///
    /// This is an easy way to get a &'static mut reference in RIOT. Its downsides (compared to
    /// cortex-m-rt's entry mechanisms) are:
    ///
    /// * It has runtime storage cost (one mutex_t)
    /// * It has runtime processing cost (primarily the accompanying unwrap which the compiler
    ///   can't know to optimize out)
    /// * It needs a good default value (can be mitigated with MaybeUninit)
    ///
    /// but then again, it's easy.
    ///
    /// ## API rationale
    ///
    /// This requires access to the original mutex and not just an acquired guard that'd be leaked
    /// in the process: The latter could also be done on a more short-lived mutex, which would then
    /// be dropped (or even leaked-and-pushed-off-the-stack) even in a locked state. (A possibility
    /// that is fine -- we sure don't want to limit mutex usage to require a Pin reference.)
    ///
    /// The function could be generalized to some generic lifetime, but there doesn't seem to be a
    /// point to it.
    pub fn try_leak(&'static self) -> Option<&'static mut T> {
        let guard = self.try_lock()?;
        core::mem::forget(guard);
        Some(unsafe { &mut *self.data.get() })
    }
}

impl<'a, T> crate::thread::ValueInThread<&'a Mutex<T>> {
    /// Get an accessor to the mutex when the mutex is available
    ///
    /// Through the [crate::thread::ValueInThread], this is already guaranteed to run in a thread
    /// context, so no additional check is performed.
    #[doc(alias = "mutex_lock")]
    pub fn lock(self) -> MutexGuard<'a, T> {
        // unsafe: All preconditions of the C function are met (not-NULL through taking a &self,
        // being initialized through RAII guarantees, thread context is in the InThread).
        unsafe { riot_sys::mutex_lock(crate::inline_cast_mut(self.mutex.get())) };
        MutexGuard { mutex: &self }
    }
}

unsafe impl<T: Send> Send for Mutex<T> {}
unsafe impl<T: Send> Sync for Mutex<T> {}

impl<T: core::default::Default> core::default::Default for Mutex<T> {
    fn default() -> Self {
        Self::new(Default::default())
    }
}

/// A lock on a mutex
///
/// Though a MutexGuard, a mutex's inner value can be mutably accessed; the creation mechanism of
/// the locks ensures that only one MutexGuard is ever available for any given Mutex.
///
/// When the lock is dropped, the mutex becomes available again.
pub struct MutexGuard<'a, T> {
    mutex: &'a Mutex<T>,
}

impl<'a, T> Drop for MutexGuard<'a, T> {
    fn drop(&mut self) {
        unsafe { riot_sys::mutex_unlock(crate::inline_cast_mut(self.mutex.mutex.get())) }
    }
}

impl<'a, T> MutexGuard<'a, T> {
    /// Put the current thread to sleep right after unlocking the mutex. This is equivalent to
    /// calling mutex_unlock_and_sleep in RIOT.
    #[doc(alias = "mutex_unlock_and_sleep")]
    pub fn unlock_and_sleep(self) {
        let m = &self.mutex.mutex;
        ::core::mem::forget(self);
        unsafe { riot_sys::mutex_unlock_and_sleep(crate::inline_cast_mut(m.get())) };
    }
}

impl<'a, T> Deref for MutexGuard<'a, T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &*self.mutex.data.get() }
    }
}

impl<'a, T> DerefMut for MutexGuard<'a, T> {
    fn deref_mut(&mut self) -> &mut T {
        unsafe { &mut *(self.mutex.data.get()) }
    }
}

impl<T> mutex_trait::Mutex for &Mutex<T> {
    type Data = T;

    fn lock<R>(&mut self, f: impl FnOnce(&mut Self::Data) -> R) -> R {
        f(&mut Mutex::lock(self))
    }
}
