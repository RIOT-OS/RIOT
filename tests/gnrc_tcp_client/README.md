Test description
==========
This test starts a client using GNRC TCP. The test is intended to
work with gnrc_tcp_server.

On startup the client tries to connect to a server waiting
for an incoming connection request. The target address and port number
can be user specified during the test build. After successful
connection establishment, the clients sends 2048 byte containing a test pattern (0xF0)
to the peer. After successful transmission, the client expects to receive 2048 byte
 with a test pattern (0xA7) from the peer. After successful verification, the connection
 termination sequence is initiated.

The test sequence above runs a configurable amount of times.

Usage (native)
==========

Build and run test:
make clean all term

Build and run test, user specified target address:
make clean all term TCP_TARGET_ADDR=<IPv6-Addr>

Build and run test, user specified target port:
make clean all term TCP_TARGET_PORT=<Port>

Build and run test, user specified amount of test cycles:
make clean all term TCP_TEST_CYLES=<Cycles>

Build and run test, fully specified:
make clean all term TCP_TARGET_ADDR=<IPv6-Addr> TCP_TARGET_PORT=<Port> TCP_TEST_CYLES=<Cycles>
