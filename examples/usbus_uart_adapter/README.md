# USBUS UART example

This example shows how to manually instantiate an USBUS thread without relying
on the auto_init module for USBUS. In this example the USBUS thread is started
with the USBUS STDIO over CDC ACM module and then as many USBUS CDC ACM to UART
interfaces allowed by both the usbus peripheral and the number of UART
peripherals available on the board.

## Requirements

A board with `periph_usbdev` is required for this. Furthermore, an available
number of UART devices is useful but not strictly required for the example.

## Usage

When started and attached to a host computer, the host computer should recognize
the device as multiple USB ACM devices. On Linux these show up as
`/dev/ttyACMn` devices, with increasing numbers. The first ttyACM device
provided by the board is attached to the local RIOT shell of the board.

The other ttyACMn devices are connected to the UART peripherals of the board.
Both the baud rate and the parity can be controlled from the host computer as
long as the MCU supports the configured setting.
