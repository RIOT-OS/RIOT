# Modbus RTU Driver Test

## Introduction
This test checks if the Modbus RTU driver is working correctly.

Contrary to the unit tests in `drivers/modbus`, this test application performs
end-to-end testing of the Modbus client and server implementation, using an
actual Modbus RTU transport.

## Set-up
There are two ways to perform this test. The test itself can be configured by
overriding several parameters. See the `Makefile` for more information.

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

### Client/server mode
Connect a single client with one or more servers.

Prepare one or more servers using the `server <id> <misbehave>` shell command.
The `id` specifies the Modbus ID of the server (defaults to 1). The `misbehave`
parameter is optional, and when set to a non-zero value, the server will
intentionally misbehave in certain situations to test the client's robustness.

Prepare a single client using the `client <servers> <iterations>` shell
command. The `servers` parameter specifies how many servers are present on the
bus (defaults to 1). The `iterations` parameter is optional (default is
infinite) and specifies how many test iterations to perform.

### Loopback mode
With two UART peripherals, use one as client and the other as server. Connect
RX to TX and vice versa. The added benefit of this mode is that the
implementation can perform additional assertions because of direct internal
memory access.

To start this mode, use the `loopback <iterations>` shell command. The
`iterations` is optional (default is infinite) and specifies how many
test iterations to perform.

There is one catch: the client and server will have the same thread priority.
When the client is sending, the server will store the incoming bytes into an
isrpipe, which must be serviced by the server thread. However, since both
threads have the same priority, the server thread will only start servicing
when the client thread waits for a response. For larger Modbus messages, this
can lead to time-outs, because the isrpipe buffer is too small to receive a
full message at once. `COUNT_REGISTERS` is therefore limited to 16, which
results in a maximum Modbus message size of at most 40 bytes, which fits the
isrpipe buffer of 64 bytes.

Giving the server thread more priority does not work, because when the server
starts responding, the client thread should have more priority. This leads to
time-outs.

## Expected result
The client will repeatedly perform Modbus write functions using a random
addresses and random count. It will then assert the result by performing the
corresponding read functions. In client/server mode, it will also select a
random server.

None of the tests ran by the client should fail, unless `misbehave` is set for
one or more servers.
