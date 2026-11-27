//! Access to [messages](https://doc.riot-os.org/group__core__msg.html) by explicit type indication
//!
//! ## Safety
//!
//! This overall method of sending and receiving messages provides no guarantees that even a
//! pure-Rust thread doesn't accidentally reuse a number or does something else to misuse
//! ContainerMsg::recognize; a better interface is WIP in the [v2] module.

use crate::libc;
use crate::thread::KernelPID;
use core::marker::PhantomData;
use core::mem::MaybeUninit;
use riot_sys::{self, kernel_pid_t, msg_receive, msg_reply, msg_send, msg_send_receive, msg_t};

#[cfg(feature = "with_msg_v2")]
pub mod v2;

/// The source of a message
// Ideally this would be represented in memory 1:1 like a KernelPID, but I can't tell Rust that a
// KernelPID has a valid range from KERNEL_PID_FIRST to KERNEL_PID_LAST and have it use that
// knowledge.
#[derive(Debug, PartialEq)]
pub enum MsgSender {
    Invalid, // = riot_sys::KERNEL_PID_UNDEF
    Thread(KernelPID),
    ISR, // = riot_sys::KERNEL_PID_ISR
}

impl MsgSender {
    fn from_pid(pid: kernel_pid_t) -> Self {
        if pid == crate::thread::KERNEL_PID_ISR {
            MsgSender::ISR
        } else {
            KernelPID::new(pid)
                .map(MsgSender::Thread)
                .unwrap_or(MsgSender::Invalid)
        }
    }
}

#[derive(Debug)]
pub enum MsgSendError {
    ReceiverNotWaiting,
    InvalidPID,
}

pub trait Msg {
    fn get_sender(&self) -> MsgSender;
    fn get_type(&self) -> u16;
    fn send(self, target: &KernelPID) -> Result<(), MsgSendError>;
    fn send_receive(self, target: &KernelPID) -> OpaqueMsg;
    fn reply(self, response: impl WrapsMsgT) -> Result<(), ()>;
}

/// Helper trait to implement Msg for the various Msg value styles. This is not supposed to be
/// public, but leaks due to the implementation.
pub trait WrapsMsgT {
    fn extract(self) -> msg_t;
    fn view(&self) -> &msg_t;
}

impl<T> Msg for T
where
    T: WrapsMsgT,
{
    fn send(self, target: &KernelPID) -> Result<(), MsgSendError> {
        let mut m = self.extract();
        match unsafe { msg_send(&mut m, target.into()) } {
            1 => Ok(()),
            0 => Err(MsgSendError::ReceiverNotWaiting),
            _ => Err(MsgSendError::InvalidPID),
        }
    }

    fn send_receive(self, target: &KernelPID) -> OpaqueMsg {
        let mut m = self.extract();
        let _ = unsafe { msg_send_receive(&mut m, &mut m, target.into()) };
        OpaqueMsg(m)
    }

    fn reply(self, response: impl WrapsMsgT) -> Result<(), ()> {
        let mut m = self.extract();
        let mut r = response.extract();
        let result = unsafe { msg_reply(&mut m, &mut r) };
        match result {
            1 => Ok(()),
            _ => Err(()),
        }
    }

    fn get_sender(&self) -> MsgSender {
        MsgSender::from_pid(self.view().sender_pid)
    }

    fn get_type(&self) -> u16 {
        self.view().type_
    }
}

/// An initialized message with inaccessible value.
pub struct OpaqueMsg(msg_t);

impl OpaqueMsg {
    pub fn receive() -> OpaqueMsg {
        let mut m: MaybeUninit<msg_t> = MaybeUninit::uninit();
        let _ = unsafe { msg_receive(m.as_mut_ptr()) };
        OpaqueMsg(unsafe { m.assume_init() })
    }
}

impl ::core::fmt::Debug for OpaqueMsg {
    fn fmt(&self, formatter: &mut ::core::fmt::Formatter) -> ::core::fmt::Result {
        formatter
            .debug_struct("OpaqueMsg")
            .field("from", &self.get_sender())
            .field("type", &self.get_type())
            .finish()
    }
}

impl WrapsMsgT for OpaqueMsg {
    fn extract(self) -> msg_t {
        self.0
    }
    fn view(&self) -> &msg_t {
        &self.0
    }
}

/// An initialized message of a type that needs no access to a value
pub struct EmptyMsg(msg_t);

impl EmptyMsg {
    pub fn new(type_: u16) -> Self {
        EmptyMsg(msg_t {
            type_,
            ..msg_t::default()
        })
    }
}

impl WrapsMsgT for EmptyMsg {
    fn extract(self) -> msg_t {
        self.0
    }
    fn view(&self) -> &msg_t {
        &self.0
    }
}

/// An initialized message of a type whose payload is numeric
#[derive(Debug)]
pub struct NumericMsg(msg_t);

impl NumericMsg {
    pub fn new(type_: u16, value: u32) -> Self {
        NumericMsg(msg_t {
            type_,
            content: riot_sys::msg_t__bindgen_ty_1 { value: value },
            ..msg_t::default()
        })
    }

    /// Interpret an opaque message as a numeric one. The caller needs to ensure that the message
    /// was initialized with a u32 value, typically by checking the message type (or receiving the
    /// message as a response) and relying on senders to properly initialize the message.
    pub unsafe fn recognize(msg: OpaqueMsg) -> Self {
        NumericMsg(msg.extract())
    }

    pub fn get_value(&self) -> u32 {
        unsafe { self.0.content.value }
    }
}

impl WrapsMsgT for NumericMsg {
    fn extract(self) -> msg_t {
        self.0
    }
    fn view(&self) -> &msg_t {
        &self.0
    }
}

pub struct ContainerMsg<T> {
    message: msg_t,
    t: PhantomData<T>,
}

// The tricky part will be creating a Container from an OpaqueMsg -- there we'll have to know for
// sure the type and lifetime involved.
//
// T needs to be Send. If you want to pass around raw pointers as RIOT does in GNRC, wrap them like
// Pktsnip.
impl<T> ContainerMsg<T>
where
    T: Sized + Send,
{
    pub fn new(type_: u16, value: T) -> Self {
        use core::mem::size_of;
        const {
            assert!(
                size_of::<T>() <= size_of::<*mut libc::c_void>(),
                "Type too large to send"
            )
        };
        ContainerMsg {
            message: msg_t {
                type_,
                content: riot_sys::msg_t__bindgen_ty_1 {
                    ptr: unsafe { ::core::mem::transmute_copy(&value) },
                },
                ..msg_t::default()
            },
            t: PhantomData,
        }
    }

    pub fn get_value(self) -> T {
        unsafe { ::core::mem::transmute_copy(&self.message.content.ptr) }
    }

    pub unsafe fn recognize(msg: OpaqueMsg) -> Self {
        ContainerMsg {
            message: msg.extract(),
            t: PhantomData,
        }
    }
}
