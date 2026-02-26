# Modbus Saul

## Introduction
This is very basic example of a Modbus RTU server implementation, that wraps
around SAUL switches and buttons.

## Set-up
Unless overridden by board configuration, connect `UART_DEV(1)` to
a Modbus RTU client at 9600 baud. Then, use that client to interact with this
server.

This server will have the identifier 1.

## Supported functions
The following functions are demonstrated in this example:

- `MODBUS_FC_READ_COILS`
- `MODBUS_FC_WRITE_SINGLE_COIL`

Only single-count requests are supported. This is only a limitation of this
example, not of the driver.

## SAUL mapping
Note that Modbus addressing is usually 1-based in documentation, but 0-based
in implementation. The addresses mentioned below are 1-based.

### Read coils
Buttons are exposed as coils. If SAUL registration 4 is a button, then it can
be accessed using a `MODBUS_FC_READ_DISCRETE_INPUTS` for address 5 with a count
of 1.

### Write single coil
Switches (typically LEDs) are exposed as coils. Similar to read coils, if SAUL
registration 2 is a switch, then it can be accessed using a
`MODBUS_FC_WRITE_SINGLE_COIL` for address 3 with a count of 1.
