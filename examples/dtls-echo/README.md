# dtls_echo example

This example shows how to use TinyDTLS with sock_udp.

## SOCK vs. Socket

This example is configured to use socks instead of sockets (over GNRC).
It's possible to use sockets, which give a more similar approach to the original
Linux version of TinyDTLS. However, this is not tested yet.

## Fast configuration (Between RIOT instances):

Preparing the logical interfaces:

    ./../../dist/tools/tapsetup/tapsetup --create 2

For the server instance:

    make all; PORT=tap1 make term
    dtlss start
    ifconfig

Do not forget to copy the IPv6 addresses!

For the client:

    PORT=tap0 make term
    dtlsc <IPv6's server address[%netif]> "DATA to send under encrypted channel!"

# Testings
## Boards

Boards that do not support the `../gnrc_networking` example are included
in the `BOARD_INSUFFICIENT_MEMORY`, plus the board `cc2650stk`.

The code has been tested in the FIT IOT-LAB tesbed with the remote
`iotlab-m3` and `iotlab-a8-m3` boards and with local `samr21-xpro` boards.

## Handling the static memory allocation

TinyDTLS for RIOT is using the `sys/memarray` module and therefore there
are certain limits. Said resources are defined in
`tinydtls/platform-specific/riot_boards.h`, but can be overwritten at
compile time. Their default values are considered for having two DTLS
contexts (for purpose of DTLS renegotiation).

The resources handled by memarray are:
* `DTLS_CONTEXT_MAX` (default 2) The maximum number of DTLS context at the
   same time.
* `DTLS_PEER_MAX` (default 1) The maximum number DTLS peers (i.e. sessions).
* `DTLS_HANDSHAKE_MAX` (default 1) The maximum number of concurrent DTLS handshakes.
* `DTLS_SECURITY_MAX` (the sum of the previous two) The maximum number of
   concurrently used cipher keys.
* `DTLS_HASH_MAX` (Default: `3 * DTLS_PEER_MAX`) The maximum number of hash
  functions that can be used in parallel.

## Handling retransmissions

By default, the number of transmissions of any DTLS record is settled to just
one. This can be handled by `DTLS_DEFAULT_MAX_RETRANSMIT` (defined in
`tinydtls/platform-specific/riot_boards.h`).
