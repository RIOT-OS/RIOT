use embassy_futures::select::{Either, select};
use embedded_nal_async::{SocketAddr, UnconnectedUdp};
use riot_wrappers::mutex::Mutex;
use riot_wrappers::{println, ztimer};
use riot_wrappers::socket_embedded_nal_async_udp::UnconnectedUdpSocket;
use rs_matter::error::{Error, ErrorCode};
use rs_matter::transport::network::{UdpReceive, UdpSend};
use embassy_sync::{
    signal::Signal,
    blocking_mutex::raw::NoopRawMutex
};
use embedded_hal_async::delay::DelayNs;

pub type Notification = Signal<NoopRawMutex, ()>;
pub type BoolNotification = Signal<NoopRawMutex, bool>;

pub struct UdpSocketWrapper {
    local_addr: SocketAddr,
    socket: Mutex<UnconnectedUdpSocket>,
    release_socket_notification: Notification,
    socket_released_notification: BoolNotification,
}

impl UdpSocketWrapper {
    pub fn new(local_addr: SocketAddr, socket: UnconnectedUdpSocket) -> Self {
        Self {
            local_addr,
            socket: Mutex::new(socket),
            release_socket_notification: Notification::new(),
            socket_released_notification: BoolNotification::new(),
        }
    }
}

impl UdpSend for &UdpSocketWrapper {
    async fn send_to(&mut self, data: &[u8], addr: SocketAddr) -> Result<(), Error> {
        if addr.is_ipv4() {
            println!("Sending to IPv4 address {} not supported!", &addr);
            return Ok(());
        }
        // tell receiver to release mutex within max. 10ms
        self.release_socket_notification.signal(());
        ztimer::Delay.delay_ms(10).await;
        let mut sock = self.socket.try_lock().expect("receiver should have ensured that this mutex is free");
        sock.send(self.local_addr, addr, data)
            .await
            .map_err(|_| Error::new(ErrorCode::StdIoError))?;
        // tell receiver sending is finished
        drop(sock);
        self.socket_released_notification.signal(true);
        Ok(())
    }
}

impl UdpReceive for &UdpSocketWrapper {
    async fn recv_from(&mut self, buffer: &mut [u8]) -> Result<(usize, SocketAddr), Error> {
        loop {
            let mut sock = self.socket.try_lock().expect("sender should have ensured that this mutex is free");
            match select(
                self.release_socket_notification.wait(),
                sock.receive_into(buffer)
            ).await {
                Either::First(cond_var) => {
                    // give up mutex for sender
                    drop(sock);
                    // ... and wait until available again
                    self.socket_released_notification.wait().await;
                    continue;
                }
                Either::Second(res) => {
                    // return receive result
                    let res = res.map(|(bytes_recvd, _, remote_addr)|
                        (bytes_recvd, remote_addr)
                    ).map_err(|_| Error::new(ErrorCode::StdIoError));
                    return res;
                }
            }
        }
    }
}