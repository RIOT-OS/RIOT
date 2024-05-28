use core::{pin::Pin, task::{Context, Poll}};
use heapless::spsc::Queue; // lock-free
use futures_util::{stream::Stream, task::AtomicWaker};
pub use futures_util::StreamExt;

#[derive(Debug)]
pub struct XStream<T: 'static, const N: usize> {
    queue: &'static mut Queue<T, N>,
    waker: &'static AtomicWaker,
}

pub type XStreamData<T, const N: usize> = (Queue<T, N>, AtomicWaker);

impl<T, const N: usize> XStream<T, N> {
    pub const fn init() -> XStreamData<T, N> {
        (Queue::<T, N>::new(), AtomicWaker::new())
    }

    pub fn get(sd: &'static mut XStreamData<T, N>) -> Self {
        let (queue, waker) = sd;

        XStream { queue, waker }
    }

    pub fn add(&mut self, item: T) {
        if self.queue.enqueue(item).is_ok() {
            self.waker.wake();
        } else {
            panic!("queue is full");
        }
    }

    #[allow(dead_code)]
    pub fn empty(&mut self) {
        while self.queue.dequeue().is_some() {}
    }

    pub fn len(&self) -> usize {
        self.queue.len()
    }
}

impl<T, const N: usize> Stream for XStream<T, N> {
    type Item = T;

    fn poll_next(mut self: Pin<&mut Self>, cx: &mut Context) -> Poll<Option<Self::Item>> {
        if let Some(item) = self.queue.dequeue() { // fast path
            return Poll::Ready(Some(item));
        }

        self.waker.register(&cx.waker());
        if let Some(item) = self.queue.dequeue() {
            self.waker.take();
            Poll::Ready(Some(item))
        } else {
            Poll::Pending
        }
    }
}