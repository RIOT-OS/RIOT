//! An implementation of the [embedded_nal] (Network Abstradtion Layer) TCP traits based on RIOT
//! sockets
//!
//! This is vastly distinct from [the UDP version](crate::socket_embedded_nal) as it requires
//! vastly different workarounds (and because it was implemented when embedded-nal had already
//! switched over to &mut stack).
//!
//! ## Warning
//!
//! The implementation of TcpExactStack is highly naïve, and may panic already with well-behaved
//! peers, let alone an adversarial one.

use core::convert::TryInto;
use core::marker::PhantomData;
use core::mem::MaybeUninit;
use core::pin::Pin;

use crate::error::{NegativeErrorExt, NumericError, ENOTCONN};

use embedded_nal::{SocketAddr, TcpClientStack, TcpFullStack};

/// A view on the RIOT socket stack that is prepared for a single listening socket that can accept
/// QUEUELEN connections simultaneously.
///
/// Note that unless CONFIG_GNRC_TCP_RCV_BUFFERS is overridden, QUEUELEN is limited to 1, anything
/// more makes it fail at setup time.
///
/// To use it as an implementation of TcpFullStack, it needs to be pinned, eg. by
/// `pin_utils::pin_mut!(stack)`, and later passed as mutable reference to the pinned item.
///
/// Note that while it would be perfectly feasible to count the number of open connection and allow
/// this to be dropped when all connections are closed, this will only be implemented once there is
/// any case that needs it (as most RIOT servers are up indefinitely).
pub struct ListenStack<const QUEUELEN: usize> {
    // This should be type state, but embedded-nal does not allow that.
    stage: ListenStage,
    listener: riot_sys::sock_tcp_queue_t,
    connections: [riot_sys::sock_tcp_t; QUEUELEN],
    // because by passing listener and connections to the socket API, we promise not to move them
    // any more
    _unpin: core::marker::PhantomPinned,
}

#[derive(PartialEq)]
enum ListenStage {
    /// No socket was populated
    New,
    /// The listener was handed out
    Bound,
}

impl<const QUEUELEN: usize> ListenStack<QUEUELEN> {
    // unsafe: We never promise not to move *that* one.
    pin_utils::unsafe_unpinned!(stage: ListenStage);
}

impl<const QUEUELEN: usize> Default for ListenStack<QUEUELEN> {
    fn default() -> Self {
        ListenStack {
            stage: ListenStage::New,
            // As this is usually one-time cost, doing the additional code dance to make this
            // uninit isn't worth it right now.
            listener: Default::default(),
            connections: [Default::default(); QUEUELEN],
            _unpin: Default::default(),
        }
    }
}

impl<const QUEUELEN: usize> Drop for ListenStack<QUEUELEN> {
    fn drop(&mut self) {
        unimplemented!("Sorry, I didn't keep track of which connections are all on")
    }
}

/// Socket for a given pool.
///
/// The lifetime is used as branding to ensure sockets are always used with their respective
/// stacks.
#[derive(Debug)]
pub struct Socket<'a> {
    socket: SocketImpl,
    _phantom: PhantomData<&'a ()>, // I'd even say &'a ListenStack if that didn't take a queuelen
}

#[derive(Debug, PartialEq)]
enum SocketImpl {
    // By the time socket() is called, we don't know yet what it'll be
    Unspecified,
    Listener,
    // Assuming no more than 256 connections. Could really be u7 to allow the others to take
    // niches.
    Connection(u8),
    // No Closed state is defined as the close operation consumes the Rust wrapper around the
    // socket anyway.
}

impl<'a, const QUEUELEN: usize> TcpClientStack for Pin<&'a mut ListenStack<QUEUELEN>> {
    type TcpSocket = Socket<'a>;
    type Error = NumericError;

    fn socket(&mut self) -> Result<Self::TcpSocket, Self::Error> {
        // Not knowing what it will be, we can't check anything yet
        Ok(Socket {
            socket: SocketImpl::Unspecified,
            _phantom: PhantomData,
        })
    }
    fn connect(
        &mut self,
        _sock: &mut Self::TcpSocket,
        _addr: SocketAddr,
    ) -> Result<(), nb::Error<Self::Error>> {
        panic!("A ListenStack can not connect out.")
    }
    fn is_connected(&mut self, sock: &Self::TcpSocket) -> Result<bool, Self::Error> {
        // FIXME: Check whether that's what is meant (or whether more checks should be done through
        // RIOT)
        Ok(match sock.socket {
            SocketImpl::Connection(_n) => true,
            _ => false,
        })
    }
    fn send(
        &mut self,
        sock: &mut Self::TcpSocket,
        buf: &[u8],
    ) -> Result<usize, nb::Error<Self::Error>> {
        let index = match sock.socket {
            SocketImpl::Connection(n) => usize::from(n),
            _ => panic!("Send on unconnected socket"),
        };
        unsafe {
            riot_sys::sock_tcp_write(
                &mut self.as_mut().get_unchecked_mut().connections[index],
                buf.as_ptr() as *const _,
                buf.len().try_into().unwrap_or(usize::MAX as _),
            )
        }
        .negative_to_error()
        .map_err(|e| e.again_is_wouldblock())
        .map(|n| n as _)
    }
    fn receive(
        &mut self,
        sock: &mut Self::TcpSocket,
        buf: &mut [u8],
    ) -> Result<usize, nb::Error<Self::Error>> {
        let index = match sock.socket {
            SocketImpl::Connection(n) => usize::from(n),
            _ => panic!("Receive on unconnected socket"),
        };
        unsafe {
            riot_sys::sock_tcp_read(
                &mut self.as_mut().get_unchecked_mut().connections[index],
                buf.as_ptr() as *mut _,
                buf.len().try_into().unwrap_or(usize::MAX as _),
                0,
            )
        }
        .negative_to_error()
        .map_err(|e| e.again_is_wouldblock())
        .map(|n| n as _)
    }
    fn close(&mut self, sock: Self::TcpSocket) -> Result<(), Self::Error> {
        let index = match sock.socket {
            SocketImpl::Connection(n) => usize::from(n),
            _ => panic!("Receive on unconnected socket"),
        };
        unsafe {
            riot_sys::sock_tcp_disconnect(&mut self.as_mut().get_unchecked_mut().connections[index])
        };
        Ok(())
    }
}

impl<'a, const QUEUELEN: usize> TcpFullStack for Pin<&'a mut ListenStack<QUEUELEN>> {
    fn bind(&mut self, sock: &mut Self::TcpSocket, port: u16) -> Result<(), Self::Error> {
        assert!(
            self.stage == ListenStage::New,
            "Stack already has its listening socket bound"
        );
        *self.as_mut().stage() = ListenStage::Bound;

        assert!(
            sock.socket == SocketImpl::Unspecified,
            "Attempted to bind running socket"
        );
        sock.socket = SocketImpl::Listener;

        // Reusing UdpEp because TcpEp is probably (FIXME) all the same.
        let local = crate::socket::UdpEp::ipv6_any().with_port(port);

        unsafe {
            riot_sys::sock_tcp_listen(
                &mut self.as_mut().get_unchecked_mut().listener,
                local.as_ref(),
                self.as_mut().get_unchecked_mut().connections.as_mut_ptr(),
                self.connections
                    .len()
                    .try_into()
                    .expect("Size exceeds expressible size"),
                0,
            )
        }
        .negative_to_error()?;

        Ok(())
    }
    fn listen(&mut self, _sock: &mut Self::TcpSocket) -> Result<(), Self::Error> {
        // Done already in bind
        Ok(())
    }
    fn accept(
        &mut self,
        // This is and can actually be ignored, because our stack object only serves a single
        // listening socket.
        _sock: &mut Self::TcpSocket,
    ) -> Result<(Self::TcpSocket, SocketAddr), nb::Error<Self::Error>> {
        let mut sockptr = core::ptr::null_mut();
        unsafe {
            riot_sys::sock_tcp_accept(
                &mut self.as_mut().get_unchecked_mut().listener,
                &mut sockptr,
                0, // return immediately / nonblocking
            )
        }
        .negative_to_error()
        .map_err(|e| e.again_is_wouldblock())?;
        // unsafe: That's what sock_tcp_accept implicitly (FIXME) promises
        let index = unsafe { sockptr.offset_from(self.connections.as_ptr()) };

        let remote = unsafe {
            let mut remote: MaybeUninit<riot_sys::sock_tcp_ep_t> = MaybeUninit::uninit();
            riot_sys::sock_tcp_get_remote(sockptr, remote.as_mut_ptr());
            remote.assume_init()
        };

        Ok((
            Socket {
                socket: SocketImpl::Connection(index.try_into().expect("Excessive pool")),
                _phantom: PhantomData,
            },
            crate::socket::UdpEp(remote).into(),
        ))
    }
}

impl<'a, const QUEUELEN: usize> embedded_nal_tcpextensions::TcpExactStack
    for Pin<&'a mut ListenStack<QUEUELEN>>
{
    const RECVBUFLEN: usize = 1152;
    const SENDBUFLEN: usize = 1152;

    fn receive_exact(
        &mut self,
        sock: &mut Self::TcpSocket,
        buf: &mut [u8],
    ) -> Result<(), nb::Error<Self::Error>> {
        let ret = self.receive(sock, buf)?;
        if ret == 0 {
            // Could mean timeout *or* connection closed, but with timeout 0 it's always
            // connection closed.
            //
            // FIXME is returning an error right here?
            return Err(nb::Error::Other(ENOTCONN));
        }
        assert!(
            ret == buf.len(),
            "Well that's a bad TcpExactStack, only got {} of {}",
            ret,
            buf.len()
        );
        Ok(())
    }
    fn send_all(
        &mut self,
        sock: &mut Self::TcpSocket,
        buf: &[u8],
    ) -> Result<(), nb::Error<Self::Error>> {
        assert!(
            self.send(sock, buf)? == buf.len(),
            "Well that's a bad TcpExactStack"
        );
        Ok(())
    }
}
