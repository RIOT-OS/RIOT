/**
@defgroup    boards_adafruit-itsybitsy-nrf52 Adafruit-Itsybitsy-nRF52
@ingroup     boards
@brief       Support for the Adafruit-Itsybitsy-nRF52

### General information

This is a small formfactor (only 1.4" long by 0.7" wide) nRF52840 board made by Adafruit.

The board features one LED (LD1: blue), a user (SW1) and a
reset button as well as 21 configurable external pins(6 of which can be analog in).

### Links

- [Overview](https://learn.adafruit.com/adafruit-itsybitsy-nrf52840-express?view=all)
- [Pinouts](https://learn.adafruit.com/adafruit-itsybitsy-nrf52840-express/pinouts)

### Flash the board

The board is flashed using its on-board [boot loader](https://github.com/adafruit/Adafruit_nRF52_Bootloader).
Adafruit has a special version of the [nrfutil](https://github.com/adafruit/Adafruit_nRF52_nrfutil) that program needs to
be installed. It can turn the binary into a suitable zip file and send it to the DFU
bootloader.

The process is automated in the usual `make flash` target.

If RIOT is already running on the board, it will automatically reset the CPU and enter
the bootloader.
If some other firmware is running or RIOT crashed, you need to enter the bootloader
manually by double tapping the board's reset button.

Readiness of the bootloader is indicated by LD1 pulsing in blue.

Important to note that Adafruit's nrfutil is not compatible with Nordic's nrfutil.

#### nrfutil installation

On systems with Python 3, a recent version of pip is required to install all dependencies;
you may need to run `pip3 install --upgrade pip3` before being able to run `pip3 install adafruit-nrfutil` successfully.

### Accessing STDIO

The usual way to obtain a console on this board is using an emulated USB serial port.


### Todo

Add support for the mini DotStar RGB LED

 */
