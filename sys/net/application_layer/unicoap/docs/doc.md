@defgroup net_unicoap unicoap: Unified CoAP Suite
@ingroup  net
@brief    Send requests and create server resources using the Constrained Application Protocol across different transports
@{

Module. Specify `USEMODULE += unicoap` in your application's Makefile.

@warning `unicoap` is work in progress. Not all functionality is implemented in RIOT yet, however
the documentation already exists. Do not expect everything to work yet.

`unicoap` is RIOT's unified and modular framework for communication via the Constrained Application
Protocol. `unicoap` supports different transports and several CoAP features, enabled by
a **layered** and **modular** design. Support for each CoAP transport, such as UDP, is available through
[drivers](@ref net_unicoap_drivers).

`unicoap` aims to eventually replace @ref net_gcoap, @ref net_nanocoap, and @ref net_nanosock,
in favor of a more beginner-friendly and easily extensible design.

## CoAP

The [Constrained Application Protocol (CoAP)](https://datatracker.ietf.org/doc/html/rfc7252)
is a lightweight alternative to HTTP. HTTP as a general-purpose application protocol carries a
significant overhead and is thus problematic for IoT networks with limited bandwidth and
nodes with little memory. CoAP covers a range of features needed in the IoT, such as resource
discovery, message fragmentation, and end-to-end message protection.

## Quick start

In your application Makefile, add
```Makefile
USEMODULE += unicoap
USEMODULE += unicoap_driver_udp
```

`unicoap` enables support for CoAP over various transport protocols. Currently, `unicoap` supports
a @ref net_unicoap_drivers_udp and @ref net_unicoap_drivers_dtls.
You must specify at least one driver to use networking functionality. If you just want to
[use message APIs](@ref net_unicoap_message_example), you can use the framing implementation of
each driver, such as the @ref net_unicoap_drivers_rfc7252_pdu submodule for the RFC 7252 PDU format.
`unicoap` implements both a client and a server.

To configure `unicoap`, go to the @ref net_unicoap_config.
For extending `unicoap`, refer to @ref net_unicoap_internal.

<!--
FIXME: undefined references, upcoming PR
If you are already using `unicoap` und want to go the extra mile, head to
@ref net_unicoap_optimizing.
-->

@}
