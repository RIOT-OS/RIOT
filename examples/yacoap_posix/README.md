# examples/yacoap_client_server

This examples shows basic usage of YaCoAP in RIOT. It shows CoAP client and
server side functions using POSIX sockets. However YaCoAP is completely
independent of any underlying send or receive API, its a plain CoAP packet
handler. So POSIX sockets can be replaced easily, e.g. using RIOTs `conn` or
future `sock` API.

## CoAP server part

This example starts a server thread that provides 2 distinct resources, namely:
`/riot/board` and `/riot/mcu`. Both can be accessed using CoAP GET requests.
Further, this example also provide the obligatory `/.well-known/core` in
standard link-format.

## CoAP client part

This example provides 2 CoAP client calls through the RIOT shell, as follows:

* `get <ip>`: retrieving `/.well-known/core` from the specified CoAP server ip
* `put <ip> <path> <content>`: sending a PUT request to the specified CoAP
   server creating or modifying resource in `path` with `content`.
