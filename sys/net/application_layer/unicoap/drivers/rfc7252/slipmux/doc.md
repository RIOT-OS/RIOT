@defgroup net_unicoap_drivers_slipmux CoAP over Slipmux Driver
@ingroup net_unicoap_drivers
@brief Use CoAP over the Slipmux protocol

Module. Specify `USEMODULE += unicoap_driver_slipmux` in your application's Makefile.

This module is typically selected by `slipdev_config` as a dependency.
You don't need to select this module directly.

@see @ref UNICOAP_PROTO_SLIPMUX

This is the dependency graph of this driver:

```
unicoap_driver_udp
├── unicoap_driver_rfc7252_common
│   ├── unicoap_driver_rfc7252_common_messaging
│   └── unicoap_driver_rfc7252_common_pdu
└── ... (operating system networking module)
```
