# U8g2 Package Test

## About
This is a test application for the U8g2 package. This package is a graphical display library, including display drivers.

## Usage
This test application will initialize the U8g2 to output on one of the following:

* output to terminal virtual screen.
* output to SDL virtual screen.
* output to I2C graphics screen.
* output to SPI graphics screen.

Note: you may have to run `make clean` after you changed to a different output mode.

### Output to terminal
To output to this virtual screen, supply `TEST_OUTPUT=1` to the `make` command.

### Output to SDL
To output to this virtual screen, supply `TEST_OUTPUT=2` to the `make` command.

This is a native-only option and requires SDL (32-bits) to be installed.

### Output to SPI
To output to screen, supply `TEST_OUTPUT=3` to the `make` command.

* `TEST_SPI` &mdash; The SPI device.
* `TEST_PIN_CS` &mdash; If applicable, the CS pin.
* `TEST_PIN_DC` &mdash; If applicable, the Data/Command pin.
* `TEST_PIN_RESET` &mdash; If applicable, the reset pin.
* `TEST_DISPLAY` &mdash; The used display driver (see https://github.com/olikraus/u8g2/wiki/u8g2setupc). Make sure you select a SPI compatible display.

### Output to I2C
To output to screen, supply `TEST_OUTPUT=4` to the `make` command.

Furthermore, you can configure the following:

* `TEST_I2C` &mdash; The I2C device.
* `TEST_ADDR` &mdash; The address to write commands to.
* `TEST_PIN_RESET` &mdash; If applicable, the reset pin.
* `TEST_DISPLAY` &mdash; The used display driver (see https://github.com/olikraus/u8g2/wiki/u8g2setupc). Make sure you select an I2C compatible display.

## Expected result
The output of this test depends on the output mode and used hardware. If it works, the application cycles through three screens with the text: 'This is RIOT-OS'.
