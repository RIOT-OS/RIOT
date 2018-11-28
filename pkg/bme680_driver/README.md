# bme680_driver

## Introduction
[bme680](https://github.com/BoschSensortec/BME680_driver) is an I2C/SPI API for BME680 sensor.

The library is written and maintained by Bosch Sensortec, it is platform independent, as long as the right drivers are available for the given MCU.

In addition, this driver can use floating point if available on your MCU. By default, this package do not use it.

## Usage
Add `USEPKG += bme680_driver` in your Makefile and `#include "bme680_hal.h"` to your code. Refer to the code documentation for more information on the API.

## RIOT-OS interface
This package uses the original source with an added interface for RIOT-OS.
I2C or SPI can be select by setting xxxx define in your code application.

In order to use floating point, you can set the ENABLE_FP variable:
`ENABLE_FP=1 make BOARD=stm32f4discovery -C tests/pkg_bme680_driver`

The following callbacks add support for the included drivers via I2C and SPI peripherals:

* `i2c_read_hal`
* `i2c_write_hal`
* `spi_read_hal`
* `spi_write_hal`

