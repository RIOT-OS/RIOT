# MH-Z19 Driver Test

## Introduction
This test will test if the MH-Z19 C02 gas sensor is working.

## Configuration

* `TEST_UART` &mdash; UART interface to use

## Expected result
The sensor should continuously (every 1 sec) output the CO2 ppm level. When the
sensor is disconnected, it should display a failure message every second.
