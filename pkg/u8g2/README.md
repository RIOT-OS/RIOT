# U8g2

## Introduction
[U8g2](https://github.com/olikraus/u8g2) is a monochrome graphics library for LCDs and OLEDs. It contains both drivers and high-level drawing routines.

The library is originally written for Arduino boards, but it runs just fine on other platforms, as long as the right drivers are available.

## Usage
Just put `USEPKG += u8g2` in your Makefile and `#include "u8g2.h"` into your code. In case you intend to use SPI/I2C, you should put `#include "u8x8_riot.h"` in your code as well, and use the methods efined there. Refer to the [U8g2 wiki](https://github.com/olikraus/u8g2/wiki) for more information on the API.

## RIOT-OS interface
This adds an interface for RIOT-OS by providing functions in u8x8_riot.h.

The following two callbacks add support for the included drivers via I2C and SPI peripherals:

* `u8x8_byte_riotos_hw_spi`
* `u8x8_byte_riotos_hw_i2c`

For timing and GPIO related operations, the following callback is available.

* `u8x8_gpio_and_delay_riotos`

U8g2 needs to map pin numbers to RIOT-OS pin numbers. It also needs to know which peripheral to use. This is acieved by setting the user_ptr in u8x8 either directly or via the u8g2_SetUserPtr() function to point to the u8x8_riot_t data structure defined in u8x8_riot.h

* `u8g2_SetPins(u8g2_dev, pins, bitmap)`
* `u8g2_SetDevice(u8g2_dev, dev)`

## Virtual displays
For targets without an I2C or SPI, virtual displays are available. These displays are part of U8g2, but are not compiled by default.

* By adding `USEMODULE += u8g2_utf8`, a terminal display is used as virtual display, using UTF8 block characters that are printed to stdout.
* By adding `USEMODULE += u8g2_sdl`, a SDL virtual display will be used. This is only available on native targets that have SDL installed. It uses `sdl2-config` to find the headers and libraries. Note that RIOT-OS builds 32-bit binaries and requires 32-bit SDL libraries.

## Example
See the code in /tests/pkg_u8g2 for a reference implementation. Note, that some defines used in the test are set in its Makefile.
