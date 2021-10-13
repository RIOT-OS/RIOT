# BME680 driver test

## About

This is a test application for the BME680 driver.
This driver depends on the Bosch Sensortech
[BME680 driver](https://github.com/BoschSensortec/BME680_driver).

## Usage

This test application will initialize one or more BME680 devices to output
the following every 5 seconds:

* Temperature
* Humidity
* Pressure
* Resistance value (depending on VOC gas)

The driver can use either fixed-point or floating-point arithmetic for all
conversions. By default fixed-point arithmetic is used. To use floating-point
arithmetic, the `bme680_fp` module has to be enabled. This can be done in the
test application by setting the environment variable `ENABLE_FP`:
```
ENABLE_FP=1 make BOARD=... -C tests/driver_bme680
```

## Interface

BME680 sensors can be used with I2C and/or SPI. Which interface is used by
which BME680 sensor is defined in the `bme680_params` parameters. The
respective implementation is enabled by the modules `bme680_i2c` and
`bme680_spi`.

Which implementation is used for the test application is defined by the
`DRIVER` environment variable. By default `bme680_i2c` is used. To use
`bme680_spi`, the `DRIVER` variable could be set at the make command line:
```
DRIVER=bme680_spi make BOARD=... -C tests/driver_bme680
```

It is also possible to use I2C as well SPI simultaneously in the test
application:
```
DRIVER=bme680_spi bme680_i2c' make BOARD=... -C tests/driver_bme680
```
