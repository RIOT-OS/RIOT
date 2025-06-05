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
[_drivers_](@ref net_unicoap_drivers).

`unicoap` aims to eventually replace [`gcoap`](@ref net_gcoap),
[`nanocoap`](@ref net_nanocoap), and [`nanocoap_sock`](@ref net_nanosock), in favor of
a more beginner-friendly and easily extensible design.

To configure `unicoap`, go to the [compile-time configuration](@ref net_unicoap_config).
<!--
FIXME: undefined references, upcoming PR
If you are already using `unicoap` und want to go the extra mile, head to
@ref net_unicoap_optimizing.
For extending or debugging internals, read more on this framework's internal mechanics in the
@ref net_unicoap_internal.
-->

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
You must specify at least one driver to use networking functionality. If you just want to
[use message APIs](@ref net_unicoap_message_example), you can use the framing implementation of
each driver, such as the
@ref net_unicoap_drivers_rfc7252_pdu submodule for the RFC 7252 PDU format used by the
@ref net_unicoap_drivers_udp and
@ref net_unicoap_drivers_dtls.

### Sending a Request as a Client
```c
...
```
<!--
FIXME: undefined references, upcoming PR
[Dive into the client API](@ref net_unicoap_client)
-->

### Processing Requests as a Server
```c
...
```
<!--
FIXME: undefined references, upcoming PR
[Read more about the server API](@ref net_unicoap_server)
-->

## Choosing a CoAP Driver
`unicoap` enables support for CoAP over various transport protocols. Currently, CoAP over UDP
and CoAP over DTLS (over UDP) are supported. To build your CoAP application, you must
specify at least one CoAP driver that implements the CoAP messaging and transport
internals.

## CoAP over UDP (RFC 7252)
To use CoAP over UDP, add
```makefile
USEMODULE += unicoap_driver_udp
```
to your Makefile.

## CoAP over DTLS (RFC 7252)
To use CoAP over DTLS, add
```makefile
USEMODULE += unicoap_driver_dtls
```
to your Makefile.

Also, include these headers required for managing DTLS credentials.
```c
#include "net/sock/dtls/creds.h"
#include "net/credman.h"
#include "net/dsm.h"
```

Then, in your application, call @ref sock_dtls_add_credential to add a DTLS credential.
<!--
FIXME: undefined references, upcoming PR
If you need
to access the DTLS socket, call @ref unicoap_transport_io_dtls_get_socket(). You must add
DTLS credentials yourself.
-->

@}
