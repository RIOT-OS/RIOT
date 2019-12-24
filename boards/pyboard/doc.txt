/**
@defgroup    boards_pyboard pyboard
@ingroup     boards
@brief       Support for the pyboard

# Overview

This port adds basic support for the MicroPython pyboard.

Currently the support is pretty minimal, and you might miss many features that
the original firmware offers. E.g., the board won't show up via USB as either
serial or mass storage device.

In order to get serial output, connect an USB/serial adapter to USART1 RX/TX
(Pins X9 / X10).

# How to flash

## put board into DFU mode:

- connect board via USB
- connect 3.3v to DFU pin (See [official docs](https://github.com/micropython/micropython/wiki/Pyboard-Firmware-Update))
- press reset button

## Flash as usual:

    $ BOARD=pyboard make flash

- unconnect 3.3v/DFU pin
- reset again

 */
