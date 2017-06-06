# About
This is a test application for the U8glib package. This package is a graphical display library, including display drivers.

# Usage
This test application will initialize the U8glib to output on one of the following:

 * output to stdout.
 * output to I2C graphics screen.
 * output to SPI graphics screen.

## Output to stdout
To output to screen, ensure add `TEST_U8GLIB_OUTPUT=1` to the `make` command.

When using a ANSI compatible terminal, one can specify `-DU8G_STDOUT_ANSI` to use ANSI characters to render the screen. Otherwise, normal characters are used.

## Output to I2C
To output to screen, ensure add `TEST_U8GLIB_OUTPUT=2` to the `make` command.

Furthermore, configure the following:

* `TEST_U8GLIB_I2C` &mdash; The I2C device.
* `TEST_U8GLIB_ADDR` &mdash; The address to write commands to.
* `TEST_U8GLIB_PIN_RESET` &mdash; If applicable, the reset pin.
* `TEST_U8GLIB_DISPLAY` &mdash; The used display driver (see https://github.com/olikraus/u8glib/wiki/device).

## Output to SPI
To output to screen, ensure add `TEST_U8GLIB_OUTPUT=3` to the `make` command.

* `TEST_U8GLIB_SPI` &mdash; The SPI device.
* `TEST_U8GLIB_PIN_CSN` &mdash; If applicable, the CSn pin.
* `TEST_U8GLIB_PIN_A0` &mdash; If applicable, the Command/Data pin.
* `TEST_U8GLIB_PIN_RESET` &mdash; If applicable, the reset pin.
* `TEST_U8GLIB_DISPLAY` &mdash; The used display driver (see https://github.com/olikraus/u8glib/wiki/device).

# Expected result
The output of this test depends on the output mode and used hardware. If it works, the application cycles through three screens with the text: 'This is RIOT-OS'.
