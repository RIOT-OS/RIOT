//! An implementation of the [embedded_nal] (Network Abstradtion Layer) UDP traits based on RIOT
//! sockets

use core::convert::TryInto;
use core::mem::MaybeUninit;

use crate::error::{NegativeErrorExt, NumericError, ENOMEM, ENOTCONN};
use crate::socket::UdpEp;

use embedded_nal::SocketAddr;

/// The operating system's network stack, used to get an implementation of
/// ``embedded_nal::UdpClientStack``.
///
/// Using this is not trivial, as RIOT needs its sockets pinned to memory for their lifetime.
/// Without a heap allocator, this is achieved by allocating all the required UDP sockets in a
/// stack object. To ensure that it is not moved, sockets on it can only be created in (and live
/// only for the duration of) a the `run` callback, which gives the actual implementation of
/// UdpClientStack.
///
/// The number of UDP sockets allocated is configurable using the UDPCOUNT const generic.
pub struct Stack<const UDPCOUNT: usize> {
    udp_sockets: heapless::Vec<riot_sys::sock_udp_t, UDPCOUNT>,
}

impl<const UDPCOUNT: usize> core::fmt::Debug for Stack<UDPCOUNT> {
    fn fmt(&self, fmt: &mut core::fmt::Formatter<'_>) -> Result<(), core::fmt::Error> {
        write!(
            fmt,
            "Stack {{ {} of {} sockets used }}",
            self.udp_sockets.len(),
            UDPCOUNT
        )
    }
}

// FIXME: This should really just use Pin like socket_embedded_nal_tcp does; unfortunately, this
// doesn't align well with the .run() API, maybe that's best just to break.
#[derive(Debug)]
pub struct StackAccessor<'a, const UDPCOUNT: usize> {
    stack: &'a mut Stack<UDPCOUNT>,
}

impl<const UDPCOUNT: usize> Stack<UDPCOUNT> {
    pub fn new() -> Self {
        Self {
            udp_sockets: Default::default(),
        }
    }

    pub fn run(&mut self, runner: impl for<'a> FnOnce(StackAccessor<'a, UDPCOUNT>)) {
        let accessor = StackAccessor { stack: self };
        runner(accessor);
        // In particular, this would require tracking of whether the sockets are closed
        unimplemented!("Allocator does not have clean-up implemented");
    }
}

pub struct UdpSocket<'a> {
    // This indirection -- not having the sock_udp_t inside UdpSocket -- is necessary because the
    // way they are created (embedded-nal .socket()) produces owned values and needs owned values
    // later -- while what we'd prefer would be producing owned values and needing pinned ones.
    //
    // See also https://github.com/rust-embedded-community/embedded-nal/issues/61
    socket: Option<&'a mut riot_sys::sock_udp_t>,
}

impl<'a> UdpSocket<'a> {
    /// Version of socket() that gives errors compatible with Self::Error
    fn access(&mut self) -> Result<*mut riot_sys::sock_udp_t, NumericError> {
        self.socket().ok_or(ENOTCONN)
    }

    /// Accessor to the inner socket pointer
    ///
    /// This can be used by users of the wrapper to alter properties of the socket, as long as that
    /// does not interfere with the wrapper's operation. It is not specified which parts that are;
    /// users of this beware that what the wrapper handles can be changed in subsequent versions.
    ///
    /// The method is safe on its own because all operations on the `*mut` are unsafe anyway
    /// (including the functions exported in riot-sys). It is not returning a &mut on the inner
    /// socket because that would allow swapping it out (which RIOT doesn't like at all).
    pub fn socket(&mut self) -> Option<*mut riot_sys::sock_udp_t> {
        self.socket.as_mut().map(|s| &mut **s as _)
    }

    /// If there is an actual socket in here, close it
    fn close(&mut self) {
        if let Some(socket) = self.socket.take() {
            unsafe { riot_sys::sock_udp_close(&mut *socket) };
        }
    }
}

impl<'a, const UDPCOUNT: usize> StackAccessor<'a, UDPCOUNT> {
    /// Take one of the stack accessor's allocated slots
    fn allocate(&mut self) -> Result<*mut riot_sys::sock_udp, NumericError> {
        // This happens rarely enough that any MaybeUninit trickery is unwarranted
        self.stack
            .udp_sockets
            .push(Default::default())
            .map_err(|_| ENOMEM)?;

        let last = self.stack.udp_sockets.len() - 1;
        Ok(&mut self.stack.udp_sockets[last] as *mut _)
    }

    /// Wrapper around sock_udp_create
    fn create(
        &mut self,
        handle: &mut UdpSocket<'a>,
        local: &UdpEp,
        remote: Option<&UdpEp>,
    ) -> Result<(), NumericError> {
        handle.close();

        let socket = self.allocate()?;

        (unsafe {
            riot_sys::sock_udp_create(
                socket,
                local.as_ref(),
                remote
                    .map(|r| {
                        let r: &riot_sys::sock_udp_ep_t = r.as_ref();
                        r as *const _
                    })
                    .unwrap_or(core::ptr::null()),
                0,
            )
        })
        .negative_to_error()?;

        // unsafe: Having an 'a mutable reference for it is OK because the StackAccessor guarantees
        // that the stack is available for 'a and won't move.
        let socket: &'a mut _ = unsafe { &mut *socket };

        handle.socket = Some(socket);

        Ok(())
    }
}

impl<'a, const UDPCOUNT: usize> embedded_nal::UdpClientStack for StackAccessor<'a, UDPCOUNT> {
    type UdpSocket = UdpSocket<'a>;
    type Error = NumericError;

    fn socket(&mut self) -> Result<UdpSocket<'a>, Self::Error> {
        Ok(UdpSocket { socket: None })
    }

    fn connect(
        &mut self,
        handle: &mut Self::UdpSocket,
        remote: SocketAddr,
    ) -> Result<(), Self::Error> {
        // unsafe: Side effect free C macros
        let local = unsafe {
            match remote {
                SocketAddr::V4(_) => riot_sys::macro_SOCK_IPV4_EP_ANY(),
                SocketAddr::V6(_) => riot_sys::macro_SOCK_IPV6_EP_ANY(),
            }
            .into()
        };

        let remote = remote.into();

        self.create(handle, &local, Some(&remote))
    }
    fn send(
        &mut self,
        socket: &mut Self::UdpSocket,
        buffer: &[u8],
    ) -> Result<(), nb::Error<Self::Error>> {
        let socket = socket.access()?;

        (unsafe {
            riot_sys::sock_udp_send(
                crate::inline_cast_mut(&mut *socket as *mut _),
                buffer.as_ptr() as _,
                buffer.len().try_into().unwrap(),
                0 as *const _,
            )
        })
        .negative_to_error()
        .map(|_| ())
        // Sending never blocks in RIOT sockets
        .map_err(|e| nb::Error::Other(e))
    }
    fn receive(
        &mut self,
        socket: &mut Self::UdpSocket,
        buffer: &mut [u8],
    ) -> Result<(usize, SocketAddr), nb::Error<Self::Error>> {
        let socket = socket.access()?;

        let mut remote = MaybeUninit::uninit();

        let read = (unsafe {
            riot_sys::sock_udp_recv(
                crate::inline_cast_mut(&mut *socket as *mut _),
                buffer.as_mut_ptr() as _,
                buffer.len().try_into().unwrap(),
                0,
                crate::inline_cast_mut(remote.as_mut_ptr() as *mut _),
            )
        })
        .negative_to_error()
        .map(|e| e as usize)
        .map_err(|e| e.again_is_wouldblock());

        // unsafe: Set by C function
        let remote = UdpEp(unsafe { remote.assume_init() });

        Ok((read?, remote.into()))
    }

    fn close(&mut self, mut socket: Self::UdpSocket) -> Result<(), Self::Error> {
        socket.close();
        Ok(())
    }
}

impl<'a, const UDPCOUNT: usize> embedded_nal::UdpFullStack for StackAccessor<'a, UDPCOUNT> {
    fn bind(&mut self, handle: &mut UdpSocket<'a>, port: u16) -> Result<(), Self::Error> {
        let local = UdpEp::ipv6_any().with_port(port);

        self.create(handle, &local, None)
    }

    fn send_to(
        &mut self,
        handle: &mut UdpSocket<'a>,
        remote: SocketAddr,
        buffer: &[u8],
    ) -> Result<(), nb::Error<Self::Error>> {
        let socket = handle.access()?;

        let remote: UdpEp = remote.into();

        (unsafe {
            riot_sys::sock_udp_send(
                crate::inline_cast_mut(&mut *socket as *mut _),
                buffer.as_ptr() as _,
                buffer.len().try_into().unwrap(),
                remote.as_ref(),
            )
        })
        .negative_to_error()
        .map(|_| ())
        // Sending never blocks in RIOT sockets
        .map_err(|e| nb::Error::Other(e))
    }
}
