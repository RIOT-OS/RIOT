use crate::async_helpers::{RiotStyleFuture, RiotStylePollStruct};
use crate::error::{NegativeErrorExt, NumericError, ENOSPC};
use crate::socket::UdpEp;
use core::mem::MaybeUninit;
use riot_sys::sock_udp_t;

#[derive(Debug)]
pub struct UdpStack {
    // It's annoying we need those not to move; just asking the user to help us out here.
    static_socket_factory: fn() -> Option<&'static mut MaybeUninit<sock_udp_t>>,
}

impl UdpStack {
    pub fn new(factory: fn() -> Option<&'static mut MaybeUninit<sock_udp_t>>) -> Self {
        Self {
            static_socket_factory: factory,
        }
    }

    /// Wrpper for sock_udp_create that pulls its immovable item right out of the factory
    fn create(
        &self,
        local: Option<UdpEp>,
        remote: Option<UdpEp>,
        flags: u16,
    ) -> Result<&'static mut sock_udp_t, NumericError> {
        let socket = (self.static_socket_factory)().ok_or(ENOSPC)?;
        Ok(unsafe {
            riot_sys::sock_udp_create(
                socket.as_mut_ptr(),
                local
                    .as_ref()
                    .map(|s| s.as_ref() as *const _)
                    .unwrap_or(core::ptr::null()),
                remote
                    .as_ref()
                    .map(|s| s.as_ref() as *const _)
                    .unwrap_or(core::ptr::null()),
                flags,
            )
            .negative_to_error()?;
            socket.assume_init_mut()
        })
    }
}

fn get_local(socket: &mut sock_udp_t) -> Result<UdpEp, NumericError> {
    let mut final_local = MaybeUninit::uninit();
    Ok(unsafe {
        riot_sys::sock_udp_get_local(socket, final_local.as_mut_ptr()).negative_to_error()?;
        final_local.assume_init()
    }
    .into())
}

impl embedded_io_async::Error for NumericError {
    fn kind(&self) -> embedded_io_async::ErrorKind {
        // FIXME there are some that do make sense here
        embedded_io_async::ErrorKind::Other
    }
}

#[derive(Debug)]
pub struct ConnectedUdpSocket {
    socket: &'static mut sock_udp_t,
}

impl Drop for ConnectedUdpSocket {
    fn drop(&mut self) {
        unsafe { riot_sys::sock_udp_close(self.socket) };
    }
}

#[derive(Debug)]
pub struct UnconnectedUdpSocket {
    socket: &'static mut sock_udp_t,
}

macro_rules! implementation_module {
    ($ena_crate:ident) => {

    use super::*;

    impl $ena_crate::UdpStack for UdpStack {
        type Error = NumericError;
        type Connected = ConnectedUdpSocket;
        // This could be done more efficiently (in particular, the send of UniquelyBound wouldn't need
        // to use aux to get the sender address in), but right now implementer efficiency is the
        // bottleneck.
        type UniquelyBound = UnconnectedUdpSocket;
        type MultiplyBound = UnconnectedUdpSocket;

        async fn connect_from(
            &self,
            local: $ena_crate::SocketAddr,
            remote: $ena_crate::SocketAddr,
        ) -> Result<($ena_crate::SocketAddr, Self::Connected), Self::Error> {
            let mut socket = self.create(Some(local.into()), Some(remote.into()), 0)?;

            let final_local = get_local(&mut socket)?;

            // FIXME: Verify that sock really narrows local address in case something unspecified was
            // passed in

            // (first tests indicate that while the UDP port is fixed, the local address is not)

            Ok((final_local.into(), ConnectedUdpSocket { socket }))
        }

        async fn bind_single(
            &self,
            local: $ena_crate::SocketAddr,
        ) -> Result<($ena_crate::SocketAddr, Self::UniquelyBound), Self::Error> {
            let mut socket = self.create(Some(local.into()), None, 0)?;

            let final_local = get_local(&mut socket)?;

            Ok((final_local.into(), UnconnectedUdpSocket { socket }))
        }

        async fn bind_multiple(
            &self,
            local: $ena_crate::SocketAddr,
        ) -> Result<Self::MultiplyBound, Self::Error> {
            let socket = self.create(Some(local.into()), None, 0)?;

            Ok(UnconnectedUdpSocket { socket })
        }
    }

    impl $ena_crate::ConnectedUdp for ConnectedUdpSocket {
        type Error = NumericError;

        async fn send(&mut self, data: &[u8]) -> Result<(), Self::Error> {
            unsafe {
                riot_sys::sock_udp_send(
                    crate::inline_cast_ref_mut(self.socket),
                    data.as_ptr() as _,
                    data.len() as _,
                    core::ptr::null(),
                )
                .negative_to_error()?
            };
            Ok(())
        }
        async fn receive_into(&mut self, buffer: &mut [u8]) -> Result<usize, Self::Error> {
            // This could be done more efficiently by ugins a ReceiveIntoArgs variant that doesn't pull
            // out a remote through a pointer and a local through an aux, but as long our bottleneck is
            // implementer efficiency, dropping that data is fine.
            RiotStylePollStruct::new(ReceiveIntoArgs {
                sock: self.socket,
                buffer,
            })
            .await
            .map(|(n, _local, _remote)| n)
        }
    }

    impl $ena_crate::UnconnectedUdp for UnconnectedUdpSocket {
        type Error = NumericError;

        async fn send(
            &mut self,
            local: $ena_crate::SocketAddr,
            remote: $ena_crate::SocketAddr,
            data: &[u8],
        ) -> Result<(), Self::Error> {
            let remote: UdpEp = remote.into();
            let local: UdpEp = local.into();
            let mut aux = riot_sys::sock_udp_aux_tx_t {
                local: local.0,
                flags: riot_sys::SOCK_AUX_SET_LOCAL as _,
                ..Default::default()
            };
            unsafe {
                riot_sys::inline::sock_udp_send_aux(
                    crate::inline_cast_ref_mut(self.socket),
                    data.as_ptr() as _,
                    data.len() as _,
                    remote.as_ref() as *const _,
                    crate::inline_cast_ref_mut(&mut aux),
                )
                .negative_to_error()?
            };
            Ok(())
        }
        async fn receive_into(
            &mut self,
            buffer: &mut [u8],
        ) -> Result<
            (
                usize,
                $ena_crate::SocketAddr,
                $ena_crate::SocketAddr,
            ),
            Self::Error,
        > {
            RiotStylePollStruct::new(ReceiveIntoArgs {
                sock: self.socket,
                buffer,
            })
            .await
        }
    }

    struct ReceiveIntoArgs<'a> {
        sock: &'a mut sock_udp_t,
        buffer: &'a mut [u8],
    }
    impl RiotStyleFuture for ReceiveIntoArgs<'_> {
        type Output = Result<
            (
                usize,
                $ena_crate::SocketAddr,
                $ena_crate::SocketAddr,
            ),
            NumericError,
        >;
        fn poll(&mut self, arg: *mut crate::libc::c_void) -> core::task::Poll<Self::Output> {
            let sock: &mut sock_udp_t = self.sock;

            // Using recv_buf so that we can get the full length, and thus deliver the first
            // slice without misleading the receiver about the length of the datagram.
            //
            // Effectively, this is the sock_udp_recv function (which is internally
            // sock_udp_recv_buf with consecutive memcpy calls) reimplemented in Rust, but with
            // the change that it counts up to the full datagram length

            let mut data = core::ptr::null_mut();
            let mut buf_ctx = core::ptr::null_mut();

            let mut cursor = 0;

            // It'd be nice if we could leave that uninitialized...
            let mut aux = riot_sys::sock_udp_aux_rx_t {
                flags: riot_sys::SOCK_AUX_GET_LOCAL as _,
                ..Default::default()
            };
            let mut remote = MaybeUninit::uninit();

            let mut auxptr = Some(&mut aux);
            let mut remoteptr = Some(&mut remote);

            loop {
                match (unsafe {
                    riot_sys::sock_udp_recv_buf_aux(
                        sock,
                        &mut data,
                        &mut buf_ctx,
                        // Return immediately
                        0,
                        // That's a mouthful for "pass this pointer, and zero the pointer after the
                        // first time" (because we only want to have this copied once, especially since
                        // it's not guaranteed that it also works in later invocations -- at least the
                        // aux part doesn't)
                        remoteptr
                            .take()
                            .map(|r| r.as_mut_ptr())
                            .unwrap_or(core::ptr::null_mut()),
                        auxptr
                            .take()
                            .map(|r| r as *mut _)
                            .unwrap_or(core::ptr::null_mut()),
                    )
                })
                .negative_to_error()
                {
                    Err(crate::error::EAGAIN) => {
                        // We could accommodate that if we kept the cursor in the
                        // ReceiveIntoArgs, but as it's probably not the async RIOT API's
                        // intention to ever do this, why waste anything on it.
                        debug_assert!(
                            cursor == 0,
                            "sock_udp_recv_buf indicated availability of
                                          partial datagram before complete reception"
                        );

                        // When setting all this up, we got a &mut self, so we can be sure that there
                        // is no other thread or task simultaneously trying to receive on this, or even
                        // doing a blocking send on it (which we currently don't do anyway).
                        //
                        // (Otherwise, we'd have to worry about overwriting a callback).
                        unsafe {
                            riot_sys::sock_udp_set_cb(sock, Some(Self::callback), arg);
                        }

                        return core::task::Poll::Pending;
                    }
                    Err(e) => {
                        debug_assert!(cursor == 0, "Late error in sock_udp_recv_buf execution");
                        return core::task::Poll::Ready(Err(e));
                    }
                    Ok(0) => {
                        // unsafe: sock_udp_recv_buf_aux was successful
                        let remote: UdpEp = unsafe { remote.assume_init() }.into();
                        assert!(aux.flags & riot_sys::SOCK_AUX_GET_LOCAL as riot_sys::sock_aux_flags_t == 0, "Sock backend must provide local addresses (should be enabled through the `sock_aux_local` module)");
                        let local: UdpEp = aux.local.into();

                        return core::task::Poll::Ready(Ok((cursor, local.into(), remote.into())));
                    }
                    Ok(n) => {
                        let n = n as _;

                        if let Some(remaining_bytes) = self.buffer.len().checked_sub(cursor) {
                            let to_be_copied = core::cmp::min(remaining_bytes, n);
                            self.buffer[cursor..cursor + to_be_copied].copy_from_slice(unsafe {
                                core::slice::from_raw_parts(data as *const u8, to_be_copied)
                            });
                        }

                        cursor += n;
                    }
                }
            }
        }
    }
    impl ReceiveIntoArgs<'_> {
        unsafe extern "C" fn callback(
            _sock: *mut sock_udp_t,
            flags: riot_sys::sock_async_flags_t,
            arg: *mut crate::libc::c_void,
        ) {
            if flags & riot_sys::inline::SOCK_ASYNC_MSG_RECV == 0 {
                // We can get stray _MSG_SENT in here (although I'm not sure those even get
                // generated currently). Waking once would do no harm, as the polling function
                // would just find that the socket is surprisingly not ready and register the
                // callback anew, but let's not kick that off if two instructions can prevent
                // it.
                return;
            }
            RiotStylePollStruct::<Self>::callback(arg);
        }
    }
    impl Drop for ReceiveIntoArgs<'_> {
        fn drop(&mut self) {
            let sock: &mut sock_udp_t = self.sock;
            unsafe {
                riot_sys::sock_udp_set_cb(sock, None, core::ptr::null_mut());
            }
            // No need to drop our fields individually, they're all just references. But if we
            // had any, we'd need to drop them only after we know that no callback will fire
            // any more.
        }
    }

    }
}

mod implementation_0_7 {
    implementation_module! {embedded_nal_async_0_7}
}
