# Modbus Driver Test

## Introduction
This test checks if the Modbus driver is working correctly. It currently only
supports Modbus RTU only.

## Set-up
There are two ways to perform this test. The test itself can be configured by
overriding several parameters. See the `Makefile` for more information.

### Master/slave mode
Connect a single master with one or more slaves. Be aware of RS-485 bus
termination and biassing.

### Loopback mode
With two UART peripherals, use one as master and the other as slave. Connect RX
to TX and vice versa. The added benefit of this mode is that the implementation
can perform additional assertions because of direct memory access.

## Expected result
The master will repeatedly perform Modbus write functions using a random
addresses and random count. It will then assert the result by performing the
corresponding read functions. In master/slave mode, it will also a random
slave.
