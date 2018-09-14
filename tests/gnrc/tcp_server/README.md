Test description
==========
This test starts a server using GNRC TCP. The test is intended to
work with gnrc_tcp_client.

On startup the server assigns a given IP-Address to its network
interface and opens a given port number waiting for a client
to connect to this port. As soon as a client connects the server
expects to receive 2048 byte containing a sequence of a test pattern (0xF0).

After successful verification, the server sends 2048 byte with a test
pattern (0xA7) to the peer. After successful transmission the connection
termination sequence is initiated.

The test sequence above runs a configurable amount of times.

Usage (native)
==========

Build and run test:
make clean all term

Build and run test, user specified local address:
make clean all term TCP_LOCAL_ADDR=<IPv6-Addr>

Build and run test, user specified local port:
make clean all term TCP_LOCAL_PORT=<Port>

Build and run test, user specified amount of test cycles:
make clean all term TCP_TEST_CYLES=<Cycles>

Build and run test, fully specified:
make clean all term TCP_LOCAL_ADDR=<IPv6-Addr> TCP_LOCAL_PORT=<Port> TCP_TEST_CYLES=<Cycles>
