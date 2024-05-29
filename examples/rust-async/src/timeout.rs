use core::{future::Future, pin::Pin, task::{Context, Poll}};
use futures_util::task::AtomicWaker;

pub struct Timeout {
    waker: Option<AtomicWaker>,
}

impl Timeout {
    pub fn new() -> Self {
        Timeout { waker: Some(AtomicWaker::new()) }
    }
}

impl Future for Timeout {
    type Output = ();

    fn poll(mut self: Pin<&mut Self>, cx: &mut Context) -> Poll<<Self as Future>::Output> {
        if let Some(waker) = self.waker.take() {
            waker.register(&cx.waker());
            //set_timeout(42, move |_| { waker.wake(); });

            Poll::Pending
        } else {
            Poll::Ready(())
        }
    }
}