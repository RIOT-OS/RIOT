use core::{pin::Pin, task::{Context, Poll}};
use conquer_once::spin::OnceCell;
use crossbeam_queue::ArrayQueue;
use futures_util::{stream::Stream, task::AtomicWaker};
pub use futures_util::StreamExt;

pub type StreamData<T> = (OnceCell<ArrayQueue<T>>, AtomicWaker);
pub const fn stream_uninit<T>() -> StreamData<T> {
    (OnceCell::uninit(), AtomicWaker::new())
}

#[derive(Debug)]
pub struct XbdStream<T: 'static> {
    queue: &'static OnceCell<ArrayQueue<T>>,
    waker: &'static AtomicWaker,
}

const QUEUE_CAP_DEFAULT: usize = 100;

impl<T> XbdStream<T> {
    pub fn new(sd: &'static StreamData<T>) -> Self {
        Self::new_with_cap(sd, QUEUE_CAP_DEFAULT)
    }

    pub fn new_with_cap(sd: &'static StreamData<T>, cap: usize) -> Self {
        let (queue, waker) = sd;
        queue.try_init_once(|| ArrayQueue::new(cap))
            .expect("XbdStream::new should only be called once");

        XbdStream { queue, waker }
    }

    pub fn get(sd: &'static StreamData<T>) -> Option<Self> {
        let (queue, waker) = sd;

        if queue.get().is_some() { // already init_once
            Some(XbdStream { queue, waker })
        } else {
            None
        }
    }

    // must not block/alloc/dealloc
    fn _add(queue: &'static OnceCell<ArrayQueue<T>>, waker: &'static AtomicWaker, item: T) {
        if let Ok(queue) = queue.try_get() {
            if let Err(_) = queue.push(item) {
                panic!("queue full");
            } else {
                waker.wake();
            }
        } else {
            panic!("queue uninitialized");
        }
    }

    pub fn add(&self, item: T) {
        Self::_add(self.queue, self.waker, item);
    }

    pub fn empty(&self) {
        let queue = self.queue.try_get().unwrap();
        while let Some(_) = queue.pop() {}
    }

    pub fn len(&self) -> usize {
        self.queue.try_get().unwrap().len()
    }
}

impl<T> Stream for XbdStream<T> {
    type Item = T;

    fn poll_next(self: Pin<&mut Self>, cx: &mut Context) -> Poll<Option<Self::Item>> {
        //crate::println!("@@ poll_next(): ^^");

        let queue = self.queue
            .try_get()
            .expect("queue not initialized");

        if let Some(item) = queue.pop() { // fast path
            //crate::println!("!! fast path");
            return Poll::Ready(Some(item));
        }
        //crate::println!("!! NOT fast path");

        self.waker.register(&cx.waker());
        match queue.pop() {
            Some(item) => {
                self.waker.take();
                Poll::Ready(Some(item))
            }
            None => Poll::Pending,
        }
    }
}