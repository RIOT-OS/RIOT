/**
@defgroup    boards_arduino-leonardo Arduino Leonardo
@ingroup     boards
@brief       Support for the Arduino Leonardo board

## Overview
The Arduino Leonardo is a microcontroller board based on the ATmega32u4.

Similar to an Arduino UNO, can be recognized by computer as a mouse or keyboard.
Otherwise it's the same. Brief descriptions of both boards are available at
the official [Arduino web site.](https://www.arduino.cc/en/Main/Boards)

It has 20 digital input/output pins (of which 7 can be used as PWM outputs and
12 as analog inputs), a 16 MHz crystal oscillator, a micro USB connection, a
power jack, an ICSP header, and a reset button. It contains everything needed
to support the microcontroller; simply connect it to a computer with a USB
cable or power it with a AC-to-DC adapter or battery to get started.

For details, please look at the [Leonardo page.](@ref boards_arduino-leonardo)

## Using the shell
The shell is using the TTL (5V) serial because the USB (CDC) communication is
not currently supported.

The TTL serial cable must be connected as described below:
- FTDI RX goes to PIN1 (TX)
- FTDI TX goes to PIN0 (RX)
- FTDI GND goes to GND

Keep in mind that the Arduino Leonardo is not automatically restarted upon
`make BOARD=arduino-leonardo`, so you'll need to hit the reset button. Please
be patient, as the bootloader waits a few seconds before starting your code.

## Flashing the device
Flashing RIOT on the Arduino Leonardo is bit different compared to the other
Arduinos, as the device cannot be restarted automatically to enter the
bootloader. Therefore, you'll need to press the reset button in time to allow
avrdude (the tool used to flash the firmware) to connect to the bootloader.

### On a Fast Host

1. Type `make BOARD=arduino-leonardo flash` but do not hit enter yet
2. Press the reset button on the Arduino Leonardo
3. Hit enter

In case the compilation takes longer than the bootloader waits before starting
the user program, flashing will fail. See below to work around

### On a Slow Host

1. Run `make BOARD=arduino-leonardo`
2. Type `make BOARD=arduino-leonardo flash-only` without hitting enter yet
3. Press the reset button on the Arduino Leonardo
4. Hit enter

## Mac Users

Mac Users have export the environment variable both `PORT` and `PROG_DEV`
to point to the device file of the TTL adapter (`PORT`) and the device file of
the Arduino Leonardo (`PROG_DEV`).

 */
