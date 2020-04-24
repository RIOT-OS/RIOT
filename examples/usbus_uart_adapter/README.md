# USBUS UART example

This example shows how to manually instantiate an USBUS thread without relying
on the auto_init module for USBUS. In this example the USBUS thread is started
with  as many USBUS CDC ACM to UART interfaces allowed by both the usbus
peripheral and the number of UART peripherals available on the board.

## Requirements

A board with `periph_usbdev` is required for this. Furthermore, at least one
UART peripheral is required for the example. UART mode config is optional and is
exposed over USB when present.

## Usage

When started and attached to a host computer, the host computer should recognize
the device as multiple USB ACM devices. On Linux these show up as
`/dev/ttyACMn` devices, with increasing numbers.

The ttyACMn devices are connected to the UART peripherals of the board. Both
the baud rate and the parity can be controlled from the host computer as
long as the MCU supports the configured setting.
