@defgroup net_unicoap_drivers_udp CoAP over UDP Driver
@ingroup net_unicoap_drivers
@brief Use CoAP over the UDP transport protocol, with optional reliability
@{

Module. Specify `USEMODULE += unicoap_driver_udp` in your application's Makefile.

<!--
FIXME: undefined references, upcoming PR
To change the UDP port `unicoap` is listening on, modify @ref CONFIG_UNICOAP_UDP_PORT.
The CoAP over UDP driver uses the same socket both for client and server functionality.

@see @ref UNICOAP_PROTO_UDP
-->
@see @ref unicoap_rfc7252_message_type_t

This is the dependency graph of this driver:

```
unicoap_driver_udp
├── unicoap_driver_rfc7252_common
│   ├── unicoap_driver_rfc7252_common_messaging
│   └── unicoap_driver_rfc7252_common_pdu
├── unicoap_sock_support
│   ├── sock_async
│   ├── sock_async_event
│   ├── sock_aux_local
│   └── sock_util
└── ... (operating system networking module)
```

@}
