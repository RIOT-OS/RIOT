# JC42 Driver Test

## Introduction
This test will test if the jc42 compatible temperature sensor is working.

## Configuration
There are three parameters to configure:

* `TEST_I2C` &mdash; I2C device to use.
* `TEST_I2C_ADDR` &mdash; The sensor address (usually 0x18).
* `TEST_I2C_SPEED` &mdash; The sensor address (usually I2C_SPEED_NORMAL).

## Expected result
The sensor should continuously (every 1 sec) output the temperature. The precision should be two digits.
