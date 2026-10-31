# Modbus Transport Test

## Introduction
This test checks if a Modbus transport is working correctly.

Contrary to the unit tests in `tests/drivers/modbus`, this test application
performs end-to-end testing of the Modbus client and server implementation,
using an actual Modbus transport.

The same application supports the Modbus ASCII, Modbus RTU and Modbus TCP
transports. The test logic is transport-agnostic, only the device set-up
differs for each transport.

## Transport selection
The transport is selected at compile time using the `MODBUS_TRANSPORT`
variable, which must be one of `ascii`, `rtu` or `tcp`. On `native`, the
default is `tcp`. All other boards default to `rtu`. For example:

```shell
MODBUS_TRANSPORT=ascii BOARD=<board> make
```

The test parameters differ per transport, and can be overridden on the command
line. Refer to the `Makefile` for more information:

- Modbus ASCII and Modbus RTU:
  - `TEST_BAUDRATE` - bus baud rate.
  - `TEST_CLIENT_UART` `TEST_SERVER_UART` - UART peripheral for each role.
  - `TEST_CLIENT_PIN_RTS` / `TEST_SERVER_PIN_RTS` - 'request to send' pins.
- Modbus TCP:
  - `TEST_SERVER_PORT` - port the server listens on and the client connects to.
  - `TEST_SERVER_ADDR` - address the client connects to (default
    `192.168.100.11`).

## Set-up
There are two ways to perform this test. The test itself can be configured by
overriding several parameters, as described above.

### Client/server mode
Connect a single client with one or more servers.

Prepare one or more servers using the `server <id> <fail> <misbehave>` shell
command. The `id` specifies the Modbus identifier of the server (defaults to
1). The fail parameter is optional, and when set to a non-zero value, the
server will halt on the first error. The `misbehave` parameter is optional too,
and when set to a non-zero value, the server will intentionally misbehave in
certain situations to test the client's robustness.

Prepare a single client using the `client <servers> <fail> <iterations>` shell
command. The `servers` parameter specifies how many servers are present on the
bus (defaults to 1). The `fail` parameter is optional, and when set to a
non-zero value, the client will halt on the first error. The `iterations`
parameter is optional (default is infinite) and specifies how many test
iterations to perform.

For the Modbus TCP transport, the client shell command is
`client <ip> <fail> <iterations>` instead. The `ip` parameter specifies the
server address to connect to (defaults to `TEST_SERVER_ADDR`).

For the serial transports, connect the devices to a shared bus (see below).
For TCP, run the server and client on two separate nodes and point the client
at the server using the `ip` parameter (or `TEST_SERVER_ADDR`).

### Loopback mode
Loopback mode runs both the client and the server on a single node, which
allows the test to perform additional assertions through direct internal memory
access.

This mode is only available for the serial transports (ASCII and RTU), and
requires two UART peripherals: use one as the client and the other as the
server, and connect RX to TX and vice versa. Modbus TCP has no single-node
loopback mode in this test, because the client and server would require a
network loopback interface.

To start this mode, use the `loopback <iterations> <fail>` shell command. The
`iterations` is optional (default is infinite) and specifies how many
test iterations to perform. The `fail` parameter is optional too, and when set
to a non-zero value, the test will halt on the first error.

Note: the client and server will have the same thread priority. When the client
is sending, the server will store the incoming bytes into an isrpipe, which
must be serviced by the server thread. However, since both threads have the
same priority, the server thread will only start servicing when the client
thread waits for a response.

For larger Modbus messages, this can lead to time-outs, because the isrpipe
buffer is too small to receive a full message at once. `COUNT_REGISTERS` is
therefore limited to 16, which results in a maximum Modbus message size of at
most 40 bytes, which fits the isrpipe buffer of 64 bytes. This constraint
applies to the serial transports only.

Giving the server thread more priority does not work, because when the server
starts responding, the client thread should have more priority. This leads to
time-outs.

## Serial bus wiring (Modbus ASCII and Modbus RTU)
A single client and server can be connected by connecting the RX and TX pins of
the UART peripheral together. When connecting multiple servers and a client,
use RS-485 transceivers and connect all devices to the same bus.

Some tips (they are obvious, but easily overlooked while debugging):

- Be aware of bus termination when using longer cables.
- Be aware of biasing, to ensure the bus is in a known state when no device is
  transmitting.
- Properly set-up the 'request to send' (RTS) pin to enable/disable
  transmission.
- Uninitialized devices can actively drive the RTS pin, effectively jamming
  the bus.

## TCP setup
For testing with Modbus TCP transport, both the client and server must have an
IP address configured. When both nodes have started and have been connected to
the same network, configure the IP address using
`ifconfig add ET0 192.168.100.11/24` on the server and
`ifconfig add ET0 192.168.100.12/24` on the client. Then use the `server`
command on the server node, and `client <server ip>` on the client.

Refer to `examples/networking/misc/lwip_ipv4` for more details on networking.

## Expected result
The client will repeatedly perform Modbus write functions using random
addresses and counts. It will then assert the result by performing the
corresponding read functions. In client/server mode, it will also select a
random server.

None of the tests run by the client should fail, unless `misbehave` is set for
one or more servers.
