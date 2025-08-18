@defgroup    boards_arduino-duemilanove Arduino Duemilanove
@ingroup     boards
@brief       Support for the Arduino Duemilanove board

## Overview
The Arduino Duemilanove ("2009") is a microcontroller board based on the
ATmega168 or ATmega328.
Only the ATmega328 version is supported by RIOT.

This board is very similar to the Arduino Uno board.
In fact, The Uno is just an evolution of the Duemilanove/Atmega328 version,
with a better USB/Serial chip and a new bootloader.
Otherwise it's the same. Brief descriptions of both boards are available at
the official [Arduino web site.](https://www.arduino.cc/en/Main/Boards)

For details, please look at the [Uno page.](@ref boards_arduino-uno)

## Flashing the device
Flashing RIOT on the Arduino Duemilanove is quite straight forward, just
connect your Arduino Uno using the programming port to your host computer and
type:

`make BOARD=arduino-duemilanove flash`

This should take care of everything!
