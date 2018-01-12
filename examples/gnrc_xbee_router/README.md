# gnrc_xbee_router example

## Requirements

In order to setup a 6LoWPAN router with XBee 802.15.4 on RIOT, you need a board
with XBee. The example application in this folder assumes as a default to be run
on an native platform using serial port (/dev/tty*) for communicating with
XBee.

If XBee is connected to a UART port other than 0, change `XBEE_PARAM_UART`
variable in the Makefile. If the baudrate is other than 9600, change
`XBEE_PARAM_BR`.

See also the `gnrc_xbee_border_router` example.
