# BME680 vendor driver

## Introduction

The [BME680_driver](https://github.com/BoschSensortec/BME680_driver) is an
I2C/SPI API for BME680 sensor.

The library is written and maintained by Bosch Sensortec. It is platform
independent, as long as the right drivers are available for the given MCU.

In addition, this driver can use floating point if available on your MCU.
By default, this package does not use it.

## Usage

Refer to the code documentation at
[GitHub](https://github.com/BoschSensortec/BME680_driver) for more information
on the API.

## RIOT-OS interface

BME680 sensors are connected either via I2C or SPI. Which interface is used by
which BME680 sensor is defined in the `bme680_params` parameters. The
respective implementation is enabled by the modules `bme680_i2c` and
`bme680_spi`. Both I2C and SPI can be used in one application.
```
USEMODULE='bme680_spi bme680_i2c' make BOARD=... -C tests/driver_bme680
```

In order to use floating point, you can enable module `bme680_fp` variable:
```
USEMODULE='bme680_fp bme680_i2c' make BOARD=... -C tests/driver_bme680
```

The following callbacks add support for the included drivers via I2C and SPI
peripherals:

* `bme680_i2c_read_hal`
* `bme680_i2c_write_hal`
* `bme680_spi_read_hal`
* `bme680_spi_write_hal`
