# Ucglib Package Test

## About
This is a test application for the Ucglib package. This package is a graphical color display library, including display drivers.

## Usage
This test application will initialize the Ucglib to output on one of the following:

* output to dummy screen.
* output to SDL virtual screen.
* output to SPI graphics screen.

Note: you may have to run `make clean` between different output modes.

### Output to dummy
To output to this virtual screen, supply `TEST_OUTPUT=1` to the `make` command.

It logs internal calls to screen and is used for testing the package internals only.

### Output to SDL
To output to this virtual screen, supply `TEST_OUTPUT=2` to the `make` command.

This is a native-only option and requires SDL2 (32-bits) to be installed.

### Output to SPI
To output to screen, supply `TEST_OUTPUT=3` to the `make` command.

* `TEST_SPI` &mdash; The SPI device.
* `TEST_PIN_CS` &mdash; If applicable, the CS pin.
* `TEST_PIN_CD` &mdash; If applicable, the Command/Data pin.
* `TEST_PIN_RESET` &mdash; If applicable, the reset pin.
* `TEST_DISPLAY` &mdash; The used display driver (see https://github.com/olikraus/ucglib/wiki/displays). Make sure you select a SPI compatible display.
* `TEST_DISPLAY_EXT` &mdash; The applicable display extension (additional commands).

## Expected result
The output of this test depends on the output mode and used hardware. If it works, the application cycles through three screens with the text: 'This is RIOT-OS'.
