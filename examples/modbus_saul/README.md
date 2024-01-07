# Modbus Saul

## Introduction
This is very basic example of a Modbus RTU slave implementation, that wraps
around SAUL.

## Set-up
Unless overridden by board configuration, connect `UART_DEV(1)` to
a Modbus RTU master at 9600 baud. Then, use this master to interact with this
slave.

This slave will have identifier 1.

## Supported functions
The following functions are implemented in this example.

- `MODBUS_FC_READ_COILS`
- `MODBUS_FC_READ_DISCRETE_INPUTS`
- `MODBUS_FC_READ_HOLDING_REGISTERS`
- `MODBUS_FC_WRITE_SINGLE_COIL`

Only single-count requests are supported. This is only a limitation of this
example, and not of the driver.

## SAUL mapping

### Holding registers
Every SAUL registration is mapped onto a Modbus address. Registration 0 is
mapped onto address 1 (1-based), registration 1 onto address 2, and so on.

Using a `MODBUS_FC_READ_HOLDING_REGISTERS` request, the SAUL registration can
be read. The response will contain the the value of the first dimension (16
bits). This works for all registrations.

Values are copied as-is: no conversion of endianness is performed.

### Discrete inputs
Buttons are exposed as discrete inputs. If SAUL registration 4 is a button,
then it can be accessed as discrete input 5 (1-based).

Discrete inputs are read-only, and can be read using a
`MODBUS_FC_READ_DISCRETE_INPUTS` request.

### Coils
Switches (typically LEDs) are exposed as coils. Similar to discrete inputs, if
SAUL registration 2 is a switch, then it can be accessed as coil 3 (1-based).

Coils can be read using `MODBUS_FC_READ_COILS` and written using
`MODBUS_FC_WRITE_SINGLE_COIL`.
