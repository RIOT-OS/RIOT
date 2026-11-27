@defgroup pkg_zenoh-pico  zenoh-pico
@ingroup  pkg
@brief    Zenoh protocol (pub/sub, queries) for constrained devices

[zenoh-pico](https://github.com/eclipse-zenoh/zenoh-pico) with a RIOT platform
layer (`ZENOH_GENERIC`): RIOT threads, mutexes and a scheduler based condition
variable, `ztimer64` clocks and TCP links over `sock_tcp`.

The build configuration is in `include/zenoh_generic_config.h`, every value
can be overridden with CFLAGS (for the package and the application). The
defaults target the client mode over TCP, e.g. towards a zenoh router
(`zenohd`) through a border router: locator `tcp/[2001:db8::1]:7447`, or
`tcp/[fe80::1%5]:7447` for a link local address of interface 5. Peer mode
needs `-DZ_FEATURE_UNICAST_PEER=1`, a listening socket accepts a single
connection.
