# U8g2

## Introduction
[U8g2](https://github.com/olikraus/u8g2) is a monochrome graphics library for LCDs and OLEDs. It contains both drivers and high-level drawing routines.

The library is originally written for Arduino's, but it runs fine on other platforms if the right drivers are available.

## Usage
Just put `USEPKG += u8g2` in your Makefile and `#include "u8g.h"` to your code.

## API
This package patches the original source to add an interface for RIOT-OS peripherals and removing most of the device/platform specific code.

The following two interfaces add add support for the included drivers via I2C and SPI peripherals:

* `u8g_com_riotos_hw_spi_init` &mdash; Interface for U8g2 included SPI displays.
* `u8g_com_riotos_ssd_i2c_init` &mdash; Interface for U8g2 included I2C SSD displays.

In addition, the following three drivers are general-purpose interfaces to write your own display driver:

* `u8g_com_riotos_i2c_init` &mdash; Generic I2C display driver interface.
* `u8g_com_riotos_spi_init` &mdash; Generic SPI display driver interface.
* `u8g_com_riotos_init` &mdash; General-purpose interface that accepts a void pointer argument.

For targets without an I2C or SPI, the following two interfaces emulate a display:

* `u8g_dev_riotos_stdout_init` &mdash; Virtual display via stdout.
* `u8g_dev_riotos_stdout_ansi_init` &mdash; Virtual display via stdout using ANSI control characters.
