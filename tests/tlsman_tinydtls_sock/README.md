# About

This test application allows to review the (D)TLS Manager (TLSMAN) module for
the tinyDTLS package.

This test consists only of a (D)TLS client side that is expected to communicate
with a server, which echoes back the messages from the client. The client will
perform the standard (D)TLS handshake, send data and then execute
renegotiations, and even closing and re-opening the connection.

The differences between the different tests should be minimum and mostly
concentrated in the Makefile files.

# (D)TLS stacks

## tinyDTLS

The pkg/tinyDTLS can be used with sock_udp (including sock_async) or socket.

For this test, tinyDTLS is tested with sock_udp. Although can be compiled with
sock_async with a sub-optimal performance.

The test is using the default PSK mode with the cipher suite
TLS_PSK_WITH_AES_128_CCM_8. Most of the default values for tinyDTLS (and RIOT)
are used, except for the following variables:

* DTLS_CONTEXT_MAX
* DTLS_PEER_MAX
* DTLS_HANDSHAKE_MAX

All of them have an impact on memory allocation and are settled by default to
one. But, for one type of renegotiation the nodes must be able to accept
temporary up to two peers.

---

# Debugging the tests

If debugging is required it's possible to enable debugging in the following
files:

- tests/tlsman_tinydtls*/main.c
- tests/tlsman_tinydtls*/client.c
- sys/net/tlsman/tlsman.c
- sys/net/tlsman/tinydtls.c (Probably the most relevant)

Additionally, it's possible to increase tinyDTLS verbosity in the Makefile by means of the following variable:

- `TINYDTLS_LOG`: Set to 0 - 6  that are equivalent to EMERG (Default), ALERT,
  CRIT, WARN, NOTICE,  INFO  and DEBUG.

# Running a valid server side

The examples/dlts-echo application is enough for this series of tests. Also, the
keys used for this test are the default one for the official tinyDTLS
repository. Therefore, the test server located in its repository is also
feasible for the testings.
