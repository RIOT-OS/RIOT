# Modbus SAUL

## Introduction

This is a very basic example of a Modbus server implementation, that wraps
around SAUL switches and buttons.

## Set-up

The default configuration will use the Modbus TCP on native boards, and
Modbus RTU for other boards. This can be overridden by setting
`MODBUS_TRANSPORT` to `ascii`, `rtu` or `tcp`.

### Modbus ASCII and Modbus RTU

Unless overridden by board configuration, connect `UART_DEV(1)` to
a Modbus RTU client at 9600 baud. Then, use that client to interact with this
server.

This server will have identifier 1.

### Modbus TCP

When Modbus TCP is selected, it will start a TCP server on port 502. It will
use lwIP for IPv4, utilizing `sock_tcp`.

Once started, configure the IP address using
`ifconfig add ET0 192.168.100.11/24`.

Refer to `examples/networking/misc/lwip_ipv4` for more details on how to set-up
the endpoint.

## Supported function codes

The following function codes are supported in this example:

- `MODBUS_FC_READ_COILS`
- `MODBUS_FC_WRITE_SINGLE_COIL`

Only single-count requests are supported. This is only a limitation of this
example, not of the driver.

## SAUL mapping

Note that Modbus addressing is usually 1-based in documentation, but 0-based
in implementation. The addresses mentioned below are 1-based.

### Read coils

Buttons are exposed as coils. If SAUL registration 4 is a button, it can then
be accessed using function code `MODBUS_FC_READ_COILS` for address 5
with a count of 1.

### Write single coil

Switches (typically LEDs) are exposed as coils. Similar to read coils, if SAUL
registration 2 is a switch, it can then be accessed using function code
`MODBUS_FC_WRITE_SINGLE_COIL` for address 3 with a count of 1.
