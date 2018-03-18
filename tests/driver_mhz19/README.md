# MH-Z19/MH-Z19B Driver Test

## Introduction
This test will test if the MH-Z19 C02 gas sensor is working. The MH-Z19B seems
to be newer design which also works with this driver.

## Configuration

* `TEST_UART` &mdash; UART interface to use

## Expected result
The sensor should continuously (every 1 sec) output the CO2 ppm level. When the
sensor is disconnected, it should display a failure message every second.
