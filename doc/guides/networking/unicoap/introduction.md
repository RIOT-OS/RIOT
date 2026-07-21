---
title: "unicoap: Unified CoAP Suite"
description: Send requests and create server resources using the Constrained Application Protocol across different transports.
---

`unicoap` is RIOT's unified and modular framework for communication via the Constrained Application
Protocol. It supports different transports and several CoAP features through a layered and
modular design. Support for each CoAP transport, such as UDP, is available through driver modules.

`unicoap` aims to eventually replace `net_gcoap`, `net_nanocoap`, and `net_nanosock`, in favor of a
more beginner-friendly and extensible design.

:::warning
`unicoap` is work in progress. Not all functionality is implemented in RIOT yet, although the
documentation already exists. Do not expect everything to work yet.
:::

## CoAP

The [Constrained Application Protocol (CoAP)](https://datatracker.ietf.org/doc/html/rfc7252)
is a lightweight alternative to HTTP. HTTP as a general-purpose application protocol carries a
significant overhead and is thus problematic for IoT networks with limited bandwidth and nodes
with little memory. CoAP covers a range of features needed in the IoT, such as resource discovery,
message fragmentation, and end-to-end message protection.

## Quick start

In your application's Makefile, add:

```makefile
USEMODULE += unicoap
USEMODULE += unicoap_driver_udp
```

`unicoap` enables support for CoAP over various transport protocols. Currently, it supports the
UDP and DTLS drivers. You must specify at least one driver to use networking functionality. If you
only want to use the message APIs, you can use the framing implementation of each driver, such as
the RFC 7252 PDU format.

`unicoap` implements both a client and a server. To create a CoAP server, see the server guide in
this folder.

For module configuration and implementation details, see the dedicated pages in this folder.
