Network API guide
=========

- Developer Memo: 3
- Author(s): Martine Lenders
- Date: January 2017

## Abstract
This memo describes and justifies the usage and existence of RIOT's various
networking APIs.


## Status
This document is a product of the community of RIOT maintainers, and aims to 
represent the consensus within this community.
The content of this document is licensed with a Creative Commons CC-BY-SA 
license.

## Terminology
This memo uses the following terminology and acronyms:

- network stack: the architecture to implement networking
- GNRC: RIOT's default network stack
- netapi: GNRC's inter-modular communication API
- netdev: RIOT's API to a network device on the MAC layer
- IPC: Inter-process communication
- sock: RIOT's primary API for network applications
- POSIX: IEEE-standardized interface for operating systems compatibility
- socket: POSIX' primary API for network applications

## 1. Introduction
There are three APIs of note to have a node communicate over the network.
From lowest level to highest these are:

- GNRC's netapi
- sock APIs
- POSIX socket API

Additionally to communicate with a network device driver directly there is
[`netdev`](http://doc.riot-os.org/group__drivers__netdev__netdev2.html) which
is not covered in detail by this document.

Choosing the right API and understanding why the others exist is crucial to
implement a memory-efficient application or networking module in RIOT. As such
this document gives a recommendation on which API to use based on their
respective design objectives.

## 2. GNRC's netapi
On the lowest level RIOT uses `netapi` to communicate with the GNRC stack. It
uses RIOT's IPC to communicate packet-wise with the network stack's individual
modules. Among other things it provides a send and a receive operation. Send is
supposed to hand packets down the network stack, receive is supposed to hand
them up. Both require all headers required by the receiving module to be
present, however, their order is different for both sending and receiving due to
[GNRC's internal structure](http://doc.riot-os.org/group__net__gnrc__pkt.html).

## 3. sock APIs
Though POSIX sockets are a very well-known API to communicate with the network
stack, their very universality make implementations of them very memory
consuming and its usage can be complicated, requiring many different function
calls to achieve minor effects. Additionally, due to their basis on numerical
file descriptors they require some form of internal state, which either results
in either dynamic memory allocation or complicated tracking of static internal
states. Both are not ideal in embedded software. With these points in mind we
designed a new set of APIs to communicate we call `sock` with the network
architecture with the following design priorities:

1. No need for dynamic memory allocation
2. User friendliness
3. Simplicity
4. Efficiency (at both front- and backend)
5. Portability

`sock` is not a singular API as sockets, but is aiming to an API for each
supported network protocol to reduce the number of function calls to do a
specific thing in a certain protocol. As such, we currently have `sock_ip` for
raw IPv4/IPv6 communication, `sock_tcp` for TCP communication and `sock_udp` for
UDP communication. This way we achieve both user friendliness and simplicity.

To prevent the need for dynamic memory allocation, the APIs are essentially
object-oriented: The state of a `sock` is always given with the function call.

## 4. POSIX socket API
Since RIOT aims to achieve some notion of POSIX-compliance it also comes with a
socket API. It is implemented using the `sock` APIs.

## 5. API recommendations
The general recommendation to users is to use the `sock` APIs for network
communication. POSIX sockets shall only be used if a third-party library or
application is ported to RIOT. `netapi` is recommended for very restricted
use-cases, where even `sock` is too general or implementations of new modules to
GNRC.
`netdev` should only be used for RAW link-layer communication and implementation
of MAC protocols.

## Acknowledgements
TODO

## References
* [POSIX sockets
  specification](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_socket.h.html)
* [netapi documentation](http://doc.riot-os.org/group__net__gnrc__netapi.html)
* [sock documentation](http://doc.riot-os.org/group__net__sock.html)
