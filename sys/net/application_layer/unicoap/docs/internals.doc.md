@defgroup net_unicoap_internal Behind The Scenes of unicoap
@ingroup net_unicoap
@{

## CoAP 101
CoAP was originally specified in [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252) and could
only be used in combination with UDP and DTLS as transport protocols.
[RFC 8223](https://datatracker.ietf.org/doc/html/rfc8323) modified the CoAP format for sending CoAP
messages over TCP, TLS, and WebSockets (including WebSockets over TLS). There is also an Internet
Draft for
[CoAP over GATT (BLE)](https://datatracker.ietf.org/doc/draft-amsuess-core-coap-over-gatt).

Each of these standards leverage different messaging models, i.e., what timeouts to apply, how
reliable transmission is implemented, and what messages are allowed to be sent
in response to a certain message type. A custom CoAP PDU header (i.e., another PDU
format) has been specified for CoAP over reliable transports.
For instance, CoAP over UDP and over DTLS share the same PDU format; so do CoAP over TCP and TLS.
The set of protocol characteristics that vary depending on the _transport_ forms a specific version
of CoAP, which is called a _CoAP combination_ in `unicoap`.
For instance, CoAP over UDP is CoAP combination, so is CoAP over DTLS.

## Layered Design

The design of `unicoap` involves three distinct layers that reflect the layered approach of CoAP,
as shown in the figure below. Conceptually, newly received message traverse these layers up to
the application, and data sent by the application travels in the opposite direction.
Located beneath the application, the _exchange_ layer embodies the REST model of CoAP.
It is responsible for handling advanced CoAP features operating above the request-response exchanges,
such as [resource observation](/FIXME-upcoming-pr-net_unicoap_client_resource_observation)
and [block-wise transfer](/FIXME-upcoming-pr-net_unicoap_blockwise).
This layer is shared between CoAP combinations, i.e., the REST semantics remain the same,
regardless of the messaging model and transport beneath.
Since messaging differs between CoAP combinations, a modular design to ease the addition
of new CoAP combinations was necessary: The layer dedicated to _messaging_ covers framing and can
accommodate a custom reliability mechanism, such as the one specified in RFC 7252
(using the four tempers `CON`, `NON`, `ACK`, `RST`). Serializing messages and parsing PDUs received
from the network are also handled by the messaging layer.
The transport layer at the bottom manages different transport protocols.
Here, `unicoap` coordinates with the operating system networking interface.

<img src="unicoap-layers.svg" alt="Figure 1: Layered Design of unicoap" width="500em"/>

### Overview of CoAP Combinations
To better illustrate what parts of the CoAP stack differ, have a look at the following graph, where
each node represents a version of a certain layer. Each leaf node stands for a different CoAP
combination ("CoAP over ...") specification.
```
                                  Requests/Responses
                             RFC 7252, RFC 7641, RFC 7959, ...
                (incl. Resource Observation, Block-Wise Transfers)
                             /                          \
                            /                            \
                           /                              \
Specification:          RFC 7252                       RFC 8323
                           |                               |
+-+- Messaging       shared between             largely shared between
| |  Model:            UDP & DTLS                TCP, TLS & WebSockets
| |                        |                       /                \
| |                        |                      /                  \
| +- PDU Format:    shared between         shared between         WebSockets
|                     UDP & DTLS             TCP & TLS             /     \
|                      /       \               /    \             /       \
|                     /         \             /      \           /         \
+-- Transport       UDP        DTLS          TCP     TLS    WebSockets  WebSockets
    Protocol:                                                           over TLS

Figure 2: Differences between CoAP combinations
```

#### CoAP over UDP and CoAP over DTLS (RFC 7252)
CoAP over UDP and DTLS works with messages of different types. A message can be confirmable (a `CON`
message), non-confirmable (`NON`), an acknowledgment message (`ACK`), or a reset message (`RST`).
Confirmable messages elicit an acknowledgement message to be sent by the peer. Hence, RFC 7252
provides optional reliability (i.e., retransmission using an exponential back-of mechanism)
using confirmable and acknowledgement messages.

@see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252)

#### CoAP over TCP, CoAP over TLS, and CoAP over WebSockets (RFC 8323)
RFC 8323 eliminates the need for reliability to be implemented on the application layer, as the underlying
transport protocol already provides reliability. While message processing looks the same for both
CoAP over TCP/TLS ([RFC 8323, Section 3](https://datatracker.ietf.org/doc/html/rfc8323#section-3)) and
CoAP over WebSockets ([RFC 8323, Section 4](https://datatracker.ietf.org/doc/html/rfc8323#section-4)),
the PDU format employed *does* vary a little between them.

@see [RFC 8323](https://datatracker.ietf.org/doc/html/rfc8323)

#### CoAP over GATT over Bluetooth Low Energy (BLE) (IETF Draft)
The [CoAP over GATT (BLE)](https://datatracker.ietf.org/doc/draft-amsuess-core-coap-over-gatt)
messaging layer works entirely different from previously specified Constrained Application Protocol variants.
Hence, the PDU format is also custom and optimized to take as little space as possible to reduce airtime.

@see [`draft-amsuess-core-coap-over-gatt`](https://datatracker.ietf.org/doc/draft-amsuess-core-coap-over-gatt)

### Drivers

To integrate new CoAP combinations, functionality for messaging and transport layer must be added.
The `unicoap` design refers to these integrations collectively as a _driver_ that represents
a CoAP combination, such as CoAP over DTLS. Each driver is a RIOT module you can import. For instance,
to use the CoAP over UDP driver, you import the `unicoap_driver_udp` by adding it to the `USEMODULE`
Makefile variable: `USEMODULE += unicoap_driver_udp`.

Drivers themselves can in turn consist of a shared module for messaging and a specific transport
support module. For example, the CoAP over DTLS driver encompasses a transport module for DTLS
networking; and depends on the common RFC 7252 messaging module also employed by the CoAP over UDP
driver. You can see this relationship in `Makefile.dep` in the `unicoap` source directory: The
common messaging module is a shared dependency of both the @ref net_unicoap_drivers_udp and
@ref net_unicoap_drivers_dtls driver module. We encourage you to follow the same approach for CoAP
combinations that share a common messaging model, such as CoAP over TCP, TLS, and WebSockets when
implementing these.

On a high level, each driver interacts with the upper layers on these three occasions:

- **Initialization and deinitialization**:
  Drivers must provide an [initialization](@ref unicoap_init) and [teardown](@ref unicoap_deinit).
  These may be used for setup work in the transport and messaging layer such as for creating
  sockets or establishing connections to peripherals, alongside allocating objects required for messaging.

- **Sending side / Outbound**: A driver must expose a standardized API for
  [sending from the messaging layer](@ref unicoap_messaging_send). The exchange layer will call into
  this functionality, prompting the driver to perform any due work in the messaging layer like
  attempting to retransmit the message. Apart from the message, as well as the remote and local
  endpoint, this function accepts flags that customize transmission behavior. The RFC 7252 message
  type is abstracted into a _reliability_ flag the messaging layer in the CoAP over UDP and DTLS
  drivers interpret as an instruction to send a confirmable message. When finished, the messaging
  layer serializes the message and forwards it to the transport implementation.

- **Receiving side / Inbound**: Upon receipt of a new message, each driver will need to invoke an
  [exchange-layer processing function](@ref unicoap_exchange_process).

- **Ping**: Due to the variability in ping mechanisms (empty `CON` in CoAP over UDP and `7.03`
  message in CoAP over reliable transports), each driver can implement a ping function. unicoap
  bundles these APIs and provides a
  [single, generic ping function that multiplexes](/FIXME-upcoming-pr-unicoap_ping) between the
  driver implementations.

### Communication Between Layers

The following figure illustrates communication between layers in a block-wise transfer,
where a client request from the application may result in multiple
[`unicoap_messaging_send`](@ref unicoap_messaging_send) and
[`unicoap_exchange_process`](@ref unicoap_exchange_process) calls between the
exchange and messaging layer:

<img src="unicoap-layers-comms.svg" alt="Figure 3: Communication between layers" width="600em"/>

The next schematic depicts how these APIs are implemented, based on the CoAP over UDP and
CoAP over DTLS drivers that share the RFC 7252 messaging implementation:

<img src="unicoap-layers-comms-apis.svg" alt="Figure 4: APIs for communication between layers" width="700em"/>

Both the CoAP over UDP and CoAP over DTLS driver support sending vectored data, hence the `sendv`
suffixes in the function names depicted in the figure above.

To manage state, the exchange and messaging layer(s) exchange notifications composed of a
notification type and an opaque state object pointer that may point to the layer-internal
state object representation. Notifications are sent using @ref unicoap_messaging_notify
and @ref unicoap_exchange_notify. The notification system serves two purposes.

First, it informs the respective other layer about the allocation and release of owned state
objects, such that a layer A can decide whether it should also release state when
layer B has just released a state object associated with a state object on layer A.
Each layer may keep references to state objects allocated in the
respective other layer --- this ought to be an opaque reference (`void*`) in most instances.
Generally, layers are not expected to know the memory layout and interface needed to control
state objects of other layers.
Layer A gets to know of opaque references to state owned by B through an allocation notification
from B.
For example, the exchange layer stores state objects called _memos_ that track an _exchange_ which
may, in turn, encompass multiple _transmissions_ on the messaging layer in the case of a block-wise
transfer. Once the messaging layer has sent a state release notification to the exchange layer,
the exchange layer may also release its memo if the block-wise transfer is done or keeps the memo
otherwise. Each time a new CoAP message is sent, the messaging layer informs the exchange layer
of any new state object allocations such that the current messaging state reference in the memo
can be set.

Second, it allows one layer to propagate errors that occurred asynchronously.
This case is called _asynchronous failure_ as synchronous failures,
i.e., those originating from a function call from the other layer, must be propagated by returning
an error instead. This is usually done by returning a negative error number, but you should check
each function's documentation.
For example, one layer may have set a timeout that once expired
leads to an asynchronous error condition that must be propagated to the other layer so it can
handle the error, i.e., by retrying or by releasing a state object it owns if it cannot recover.
Triggers for these asynchronous scenarios may be the reception of an inbound PDU on the messaging
layer, or the invocation of an API above the exchange layer. Crucially, should an error leading
to state release occur on one layer, it may decide between a regular state release notification
to the other or an error notification, depending on the implications. For example, if the failure
condition is the user calling _cancel_ on a pending request, the exchange layer may determine
it is better to release state in an orderly fashion, i.e., to communicate a regular state release
notification to the messaging layer instead of an async failure notification as the messaging
layer may tear down transport connections in the latter instance. In general, you should
question if an error on layer A is really relevant to layer B or if communicating the mere
consequence of the error on A
(regular state release notification instead of async failure notification)
suffices to cause an associated state object on B to be discarded.


## Adding a New Driver

In the `unicoap` codebase you will encounter several marks (`MARK: ...`)
that help with extending the suite.

- **MARK: unicoap_driver_extension_point**: Every region of code that would need to be extended to
  support a new transport protocol or driver is annotated with this mark.

@}
