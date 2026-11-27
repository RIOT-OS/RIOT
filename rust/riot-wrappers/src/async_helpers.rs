//! Tools used internally to create futures more easily

use core::future::Future;

/// A trait similar to Future that is practical to implement for the typical RIOT situations where
/// a waker needs to be converted into a function and argument pointer.
///
/// Wrapped in a [RiotStylePollStruct], it implements [Future], and the conversion between the arg
/// pointer and the full struct is taken care of. That wrapper may also do any optimizations such
/// as not really storing the waker if it can be compressed to a single word instead.
///
/// ## Implementing
///
/// While this can legally be implemented without unsafe, practical use will require unsafe, and
/// that requires sticking to the rules:
///
/// * Whenever [.poll()][Self::poll()] is called, do whatever the future needs to do after having been awoken. If
///   this returns [core::task::Poll::Pending] (and the future wants to be polled ever again), it
///   must then pass on the `arg` to some RIOT callback setter together with a static function of a
///   suitable signature. Conventionally, that function is called `Self::callback()`.
///
/// * When that callback function is called (and has any arguments), it may inspect the arguments
///   to decide to return early (for example, if it receives "chatter" that is unrelated to the
///   completion of the future). If it decides that this is now the callback that should make
///   progress, it must call [`RiotStylePollStruct::<Self>::callback(arg)`], with `arg` being the
///   value that was passed around through RIOT from the poll function.
///
/// * To the author's knowledge, the mechanism itself has no requirements of not shuffling any
///   items in and out of any `&mut` that are involved (otherwise, they would be pinned). However,
///   the callback mechanism itself may require no such shuffling to occur, in which case it is the
///   implementor's responsibility to not just move its data around.
pub(crate) trait RiotStyleFuture {
    type Output;
    fn poll(&mut self, arg: *mut crate::libc::c_void) -> core::task::Poll<Self::Output>;
}

/// Wrapper that makes a [Future] out of a [RiotStyleFuture] (see there for usage)
// FIXME: I'm not sure the presence and absence of #[pin] is right about these ones, but anyway,
// given they're not pub, and this module is what captures the unsafety guarantees (assisted by the
// requirements on RiotStyleFuture), this should be no worse than manually safe-declaring any
// access to args and waker.
#[pin_project::pin_project]
pub(crate) struct RiotStylePollStruct<A: RiotStyleFuture> {
    // The order of these is important: args is dropped first, thereby unregistering any callbacks.
    // Only then, the waker too can be dropped.
    args: A,
    // We can probably save that one if we rely on the waker pointing to a task, but let's not
    // force this on the system yet. (The TaskRef is short enough we could store it in the argument
    // of the callback).
    waker: Option<core::task::Waker>,
}
impl<A: RiotStyleFuture> RiotStylePollStruct<A> {
    pub(crate) fn new(args: A) -> Self {
        Self { args, waker: None }
    }

    /// Reconstruct a Self and run its waker (if one is present)
    pub(crate) unsafe fn callback(arg: *mut crate::libc::c_void) {
        // Actually Pin<>, but we just promise not to move it.
        let f: &mut Self = &mut *(arg as *mut _);
        // If it fires multiple times, we ignore it (the waker has been taken) -- unless the future
        // has been polled again, there is no use in waking for it multiple times. (We could also
        // remove the callback, but who knows how costly that might be).
        f.waker.take().map(|w| w.wake());
    }
}
impl<A: RiotStyleFuture> Future for RiotStylePollStruct<A> {
    type Output = A::Output;
    fn poll(
        mut self: core::pin::Pin<&mut Self>,
        context: &mut core::task::Context<'_>,
    ) -> core::task::Poll<Self::Output> {
        let arg = unsafe { self.as_mut().get_unchecked_mut() } as *mut Self as *mut _;
        match self.args.poll(arg) {
            core::task::Poll::Pending => {
                // Actually we only need to do that if we're returning Pending
                self.waker = Some(context.waker().clone());

                core::task::Poll::Pending
            }
            ready => ready,
        }
    }
}
