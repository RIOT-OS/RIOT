# AMS CCS811 device driver test application

## About

This is a manual test application for the CCS811 driver. It shows how the
sensor can be used for periodic polling.

## Usage

The test application demonstrates the use of the CCS811 using

- data-ready status function `ccs811_data_ready` to wait for new data and
- default configuration parameters, that is, the measurement mode
  `CCS811_MODE_1S` with one measurement per second.

Please refer `$(RIOTBASE)/tests/driver_ccs811_full` to learn how
to use the CCS811 with interrupts.
