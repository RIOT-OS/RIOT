//! Zero-sized types for threads to document that something is done (often, done the first time)
//! inside the thread.

use core::marker::PhantomData;
use core::mem::MaybeUninit;

/// Data created for each thread that is spawned.
///
/// It encodes for permission to do anything that can only be done once per thread.
pub type StartToken = TokenParts<true, true, true>;

/// Data necessary to return from a thread that has received the [StartToken] permissions.
///
/// This is created from the initials using [TokenParts::can_end()] to erase any left-over
/// information, and certifies that no actions have been taken that forbid the thread from ever
/// terminating (or if they have been taken, they have been undone).
pub struct EndToken {
    _not_send: PhantomData<*const ()>,
}

/// A [StartToken] that has possibly already lost some of its properties.
///
/// Note that while this item shows up in the documentation, the type is actually hidden and only
/// named as [StartToken]. This ensures that more properties can be added compatibly (because no
/// user ever names the type and would thus run into conflicts when the list of generics grows).
/// This has the downside that TokenParts can not easily be passed to downstream functions, and all
/// splitting has to happen at the top level; this should not be a problem in practice.
///
/// The individual parameters are:
///
/// * `MSG_SEMANTICS`: If this is true, the thread has not assigned semantics to messages it would receive yet.
/// * `MSG_QUEUE`: If this is true, the thread has not yet set up a message queue.
/// * `FLAG_SEMANTICS`: If this is true, the thread has not assigned semantics to flags yet.
///
/// (FLAG_SEMANTICS are not used yet, but are already prepared for a wrapper similar to `msg::v2`).
pub struct TokenParts<const MSG_SEMANTICS: bool, const MSG_QUEUE: bool, const FLAG_SEMANTICS: bool>
{
    pub(super) _not_send: PhantomData<*const ()>,
}

impl TokenParts<true, true, true> {
    /// Claim that the current thread has not done anything yet that is covered by this type
    ///
    /// Do not call yourself; this needs to be public because [`riot_main!`](crate::riot_main!) is
    /// a macro and thus technically called from the main crate.
    pub unsafe fn new() -> Self {
        TokenParts {
            _not_send: PhantomData,
        }
    }
}

impl<const MS: bool, const MQ: bool, const FS: bool> TokenParts<MS, MQ, FS> {
    /// Extract a token that states that code that has access to it is running in a thread (and not
    /// in an interrupt).
    pub fn in_thread(&self) -> InThread {
        // unsafe: TokenParts is not Send, so we can be sure to be in a thread
        unsafe { InThread::new_unchecked() }
    }
}

impl<const MQ: bool, const FS: bool> TokenParts<true, MQ, FS> {
    /// Extract the claim that the thread was not previously configured with any messages that
    /// would be sent to it.
    ///
    /// ## Example
    ///
    /// ```
    /// # #![no_std]
    /// # #![feature(start)]
    /// # #[start]
    /// # fn main(_argc: isize, _argv: *const *const u8) -> isize { panic!("Doc tests are not supposed to be run") }
    /// # use riot_wrappers::thread::*;
    /// fn thread(tok: StartToken) -> EndToken {
    ///     let (tok, semantics) = tok.take_msg_semantics();
    ///     // keep working with semantics and start receiving messages
    ///     //
    ///     // receive messages
    ///     //
    ///     // recover semantics when everyone has returned the license to send messages
    ///     let tok = tok.return_msg_semantics(semantics);
    ///     tok.can_end()
    /// }
    /// ```
    #[cfg(feature = "with_msg_v2")]
    pub fn take_msg_semantics(
        self,
    ) -> (
        TokenParts<false, MQ, FS>,
        crate::msg::v2::NoConfiguredMessages,
    ) {
        (
            TokenParts {
                _not_send: PhantomData,
            },
            // unsafe: This is the only safe way, and by construction running only once per thread.
            // The thread can't terminate because if it takes TokenParts it has to return an
            // end token
            unsafe { crate::msg::v2::NoConfiguredMessages::new() },
        )
    }
}

impl<const MQ: bool, const FS: bool> TokenParts<false, MQ, FS> {
    /// Inverse of [TokenParts::take_msg_semantics], indicating that the thread may be terminated again as far
    /// as message semantics are concerned.
    #[cfg(feature = "with_msg_v2")]
    pub fn return_msg_semantics(
        self,
        semantics: crate::msg::v2::NoConfiguredMessages,
    ) -> TokenParts<true, MQ, FS> {
        drop(semantics);
        TokenParts {
            _not_send: PhantomData,
        }
    }
}

impl<const MS: bool, const FS: bool> TokenParts<MS, true, FS> {
    /// Set up a message queue of given size N, and run the function `f` after that has been set
    /// up. `f` gets passed all the remaining thread invariants.
    ///
    /// As this doesn't deal with the message semantics, it can't be sure whether at function
    /// return time all other system components have stopped sending messages; the easy way out is
    /// to require the function to diverge.
    ///
    /// ## Example
    ///
    /// ```
    /// # #![no_std]
    /// # #![feature(start)]
    /// # #[start]
    /// # fn fake_start(_argc: isize, _argv: *const *const u8) -> isize { panic!("Doc tests are not supposed to be run") }
    /// # use riot_wrappers::thread::*;
    /// fn thread(tok: StartToken) -> EndToken {
    ///     tok.with_message_queue::<4, _>(|tok| {
    ///         loop {
    ///             // ...
    ///         }
    ///     })
    /// }
    /// ```
    // Could also be
    //     pub fn with_message_queue<const N: usize, F: FnOnce(TokenParts<MS, false>) -> (R, crate::msg::v2::NoConfiguredMessages), R>(self, f: F) -> (R, TokenParts<true, false>) {
    // but not only would this be harder on the closure expression side, it's also slightly
    // unsound: The `true` MS would mean that the NoConfiguredMessages could be taken out again and
    // used to configure semantics, and then all of a sudden the still-configured message queue
    // would be sent to again.
    pub fn with_message_queue<
        const N: usize,
        F: FnOnce(TokenParts<MS, false, FS>) -> crate::never::Never,
    >(
        self,
        f: F,
    ) -> ! {
        const {
            assert!(
                N.count_ones() == 1,
                "Message queue sizes need to be powers of 2"
            )
        };
        let mut queue: MaybeUninit<[riot_sys::msg_t; N]> = MaybeUninit::uninit();
        // unsafe: All requirements of the C function are satisfied
        unsafe { riot_sys::msg_init_queue(queue.as_mut_ptr() as _, N as _) };
        f(TokenParts {
            _not_send: PhantomData,
        })
    }
}

impl<const MQ: bool> TokenParts<true, MQ, true> {
    /// Certify that nothing has been done in this thread that precludes the termination of the
    /// thread
    ///
    /// MessageSemantics need to have been returned (or never taken) for the next thread on this
    /// PID would get weird messages.
    ///
    /// The MessageQueue is not checked for -- as all MessageSemantics have been returned (and thus
    /// nothing can safely send messages any more), even if there is a queue somewhere on the
    /// stack, it wouldn't be touched by others any more. (Of course, that's moot with the current
    /// mechanism of [TokenParts::with_message_queue()] as that diverges anyway).
    pub fn can_end(self) -> EndToken {
        EndToken {
            _not_send: PhantomData,
        }
    }
}

/// Zero-size statement that the current code is not running in an interrupt
#[derive(Copy, Clone, Debug)]
pub struct InThread {
    _not_send: PhantomData<*const ()>,
}

/// Zero-size statement that the current code is running in an interrupt
#[derive(Copy, Clone, Debug)]
pub struct InIsr {
    _not_send: PhantomData<*const ()>,
}

impl InThread {
    unsafe fn new_unchecked() -> Self {
        InThread {
            _not_send: PhantomData,
        }
    }

    /// Check that the code is running in thread mode
    ///
    /// Note that this is actually running code; to avoid that, call [`TokenParts::in_thread()`],
    /// which is a purely type-level procedure.
    pub fn new() -> Result<Self, InIsr> {
        match crate::interrupt::irq_is_in() {
            true => Err(unsafe { InIsr::new_unchecked() }),
            false => Ok(unsafe { InThread::new_unchecked() }),
        }
    }

    /// Wrap a `value` in a [`ValueInThread`]. This makes it non-Send, but may make additional
    /// (safe) methods on it, using the knowledge that it is still being used inside a thread.
    pub fn promote<T>(self, value: T) -> ValueInThread<T> {
        ValueInThread {
            value,
            _in_thread: self,
        }
    }
}

impl InIsr {
    unsafe fn new_unchecked() -> Self {
        InIsr {
            _not_send: PhantomData,
        }
    }

    /// Check that the code is running in IRQ mode
    pub fn new() -> Result<Self, InThread> {
        match InThread::new() {
            Ok(i) => Err(i),
            Err(i) => Ok(i),
        }
    }
}

/// A value combined with an [`InThread`] marker
///
/// This does barely implement anything on its own, but the module implementing `T` might provide
/// extra methods.
///
/// This makes the wrapped value not `Send`.
// Making the type fundamental results in ValueInThread<&Mutex<T>> being shown at Mutex's page.
#[derive(Copy, Clone)]
#[cfg_attr(feature = "nightly_docs", fundamental)]
pub struct ValueInThread<T> {
    value: T,
    _in_thread: InThread,
}

impl<T> ValueInThread<T> {
    /// Extract the wrapped value
    ///
    /// This does not produce the original `in_thread` value; these are easy enough to re-obtain or
    /// to keep a copy of around.
    pub fn into_inner(self) -> T {
        self.value
    }
}

impl<T> core::ops::Deref for ValueInThread<T> {
    type Target = T;

    fn deref(&self) -> &T {
        &self.value
    }
}

impl<T> core::ops::DerefMut for ValueInThread<T> {
    fn deref_mut(&mut self) -> &mut T {
        &mut self.value
    }
}
