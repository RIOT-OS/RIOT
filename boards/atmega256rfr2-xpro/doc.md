@defgroup    boards_atmega256rfr2-xpro Atmega256RFR2 Xplained Pro
@ingroup     boards
@brief       Support for the Atmega256RFR2 Xplained Pro board

### General information

The [Atmega256RFR2 Xplained Pro](https://www.microchip.com/DevelopmentTools/ProductDetails/ATMEGA256RFR2-XPRO)
is an evaluation kit by Microchip for their ATmega256RFR2 microcontroller.

### Flash the board

You can flash the board using the on-board EDBG programmer via JTAG. Avrdude has
support for programming an AVR via EDBG with its xplainedpro programmer.

First, make sure the default fuse settings are correct. In particular, the low
byte fuse are enabling the use of the on-board 16MHz external oscillator.<br/>

WARNING: setting the fuses incorrectly can brick your board!
```
avrdude -p m256rfr2 -c xplainedpro -U efuse:w:0xFF:m
avrdude -p m256rfr2 -c xplainedpro -U hfuse:w:0x1F:m
avrdude -p m256rfr2 -c xplainedpro -U lfuse:w:0xFF:m
```

To flash the board, just call `make` from an application directory with the
`flash` target:

```
make BOARD=atmega256rfr2-xpro -C examples/basic/hello-world flash
```

### Accessing STDIO via UART

STDIO can be accessed through the USB connector. The on-board UART-USB
adapter is not affected by flashing. It shows up as /dev/ttyACM0 on Linux.
It will be used automatically with `make term`:
```
make BOARD=atmega256rfr2-xpro -C examples/basic/hello-world term
```
