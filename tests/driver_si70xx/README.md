# Si70xx Driver Test

## Introduction
This test will test if the Si7006/13/20/21 temperature and humidity sensor is working.

## Configuration
There are three parameters to configure:

* `TEST_I2C` &mdash; I2C device to use.
* `TEST_I2C_ADDR` &mdash; The sensor address (usually 0x80 or 0x81).
* `TEST_PIN_EN` &mdash; If required, toggle the enable pin via this GPIO pin (see `GPIO_PIN` macro for your board).

## Expected result
The sensor should continuously (every 1 sec) output the humidity and temperature. The precision should be two digits.
