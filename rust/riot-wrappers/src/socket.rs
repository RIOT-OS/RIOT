/// Wrappers for elements of the Sock API

pub struct UdpEp(pub(crate) riot_sys::sock_udp_ep_t);

impl UdpEp {
    #[doc(alias = "SOCK_IPV6_EP_ANY")]
    pub fn ipv6_any() -> Self {
        // unsafe: Side effect free C macro
        unsafe { riot_sys::macro_SOCK_IPV6_EP_ANY().into() }
    }

    #[doc(alias = "SOCK_IPV4_EP_ANY")]
    pub fn ipv4_any() -> Self {
        // unsafe: Side effect free C macro
        unsafe { riot_sys::macro_SOCK_IPV4_EP_ANY().into() }
    }

    pub fn with_port(mut self, port: u16) -> Self {
        self.0.port = port;
        self
    }
}

impl From<riot_sys::sock_udp_ep_t> for UdpEp {
    fn from(ep: riot_sys::sock_udp_ep_t) -> Self {
        Self(ep)
    }
}

impl From<riot_sys::inline::sock_udp_ep_t> for UdpEp {
    fn from(ep: riot_sys::inline::sock_udp_ep_t) -> Self {
        // INLINE TRANSMUTE
        unsafe { Self(core::mem::transmute(ep)) }
    }
}

impl AsRef<riot_sys::sock_udp_ep_t> for UdpEp {
    fn as_ref(&self) -> &riot_sys::sock_udp_ep_t {
        &self.0
    }
}

impl AsMut<riot_sys::sock_udp_ep_t> for UdpEp {
    fn as_mut(&mut self) -> &mut riot_sys::sock_udp_ep_t {
        &mut self.0
    }
}

impl AsRef<riot_sys::inline::sock_udp_ep_t> for UdpEp {
    fn as_ref(&self) -> &riot_sys::inline::sock_udp_ep_t {
        unsafe { crate::inline_cast_ref(&self.0) }
    }
}

impl AsMut<riot_sys::inline::sock_udp_ep_t> for UdpEp {
    fn as_mut(&mut self) -> &mut riot_sys::inline::sock_udp_ep_t {
        unsafe { crate::inline_cast_ref_mut(&mut self.0) }
    }
}

impl Into<riot_sys::sock_udp_ep_t> for UdpEp {
    fn into(self) -> riot_sys::sock_udp_ep_t {
        self.0
    }
}

// Gated to its users to avoid dead code warnings
#[cfg(any(feature = "with_embedded_nal", feature = "with_embedded_nal_async"))]
macro_rules! implementation_no_std_net {
    ($nsn_crate:ident) => {
        use super::*;
        use $nsn_crate::SocketAddr;

        impl Into<UdpEp> for &SocketAddr {
            fn into(self) -> UdpEp {
                use SocketAddr::*;

                // Constructing via default avoids using the volatile names of the union types
                let mut ep: riot_sys::sock_udp_ep_t = Default::default();
                ep.port = self.port();

                #[cfg(accessible_riot_sys_sock_ep_t_member_ip)]
                // unsafe: C unions provide backwards compatibility aliases which are kept in sync during migration
                let ip = unsafe { &mut ep.__bindgen_anon_1.__bindgen_anon_1 };
                #[cfg(not(accessible_riot_sys_sock_ep_t_member_ip))]
                let ip = &mut ep;

                ip.family = match self {
                    V4(_) => riot_sys::AF_INET as _,
                    V6(_) => riot_sys::AF_INET6 as _,
                };
                ip.netif = match self {
                    V4(_) => 0,
                    V6(a) => a.scope_id() as _,
                };
                match self {
                    V4(a) => {
                        ip.addr.ipv4 = a.ip().octets();
                    }
                    V6(a) => {
                        ip.addr.ipv6 = a.ip().octets();
                    }
                }

                UdpEp(ep)
            }
        }

        impl Into<UdpEp> for SocketAddr {
            fn into(self) -> UdpEp {
                (&self).into()
            }
        }

        impl Into<SocketAddr> for &UdpEp {
            fn into(self) -> SocketAddr {
                #[cfg(accessible_riot_sys_sock_ep_t_member_ip)]
                // unsafe: C union provides backwards compatibility aliases which are kept in sync during migration
                let ip = unsafe { &self.0.__bindgen_anon_1.__bindgen_anon_1 };
                #[cfg(not(accessible_riot_sys_sock_ep_t_member_ip))]
                let ip = &self.0;

                match ip.family as _ {
                    riot_sys::AF_INET6 => $nsn_crate::SocketAddrV6::new(
                        // unsafe: Access to C union whose type was just checked
                        unsafe { ip.addr.ipv6.into() },
                        self.0.port,
                        0,
                        ip.netif.into(),
                    )
                    .into(),

                    riot_sys::AF_INET => $nsn_crate::SocketAddrV4::new(
                        // unsafe: Access to C union whose type was just checked
                        unsafe { ip.addr.ipv4.into() },
                        self.0.port,
                    )
                    .into(),

                    _ => panic!("Endpoint not expressible in no_std_net"),
                }
            }
        }

        impl Into<SocketAddr> for UdpEp {
            fn into(self) -> SocketAddr {
                (&self).into()
            }
        }
    };
}

#[cfg(feature = "with_embedded_nal")]
mod implementation_no_std_net_0_5 {
    implementation_no_std_net! {no_std_net_0_5}
}

#[cfg(feature = "with_embedded_nal_async")]
mod implementation_no_std_net_0_6 {
    implementation_no_std_net! {no_std_net_0_6}
}
