# AMS CCS811 device driver test application

## About

This is a manual test application for the CCS811 driver. It shows how the
sensor can be used with interrupts.

**Please note:** The interrupt pin has to be defined for this test application.

## Usage

The test application demonstrates the use of the CCS811 and pseudomodule
`ccs811_full` using

- data-ready interrupt `CCS811_INT_DATA_READY` and
- default configuration parameters, that is, the measurement mode
  `CCS811_MODE_1S` with one measurement per second.

The default configuration parameter for the interrupt pin has to be
overridden according to the hardware configuration by defining
`CCS811_PARAM_INT_PIN` before `ccs811_params.h` is included, e.g.,
```
#define CCS811_PARAM_INT_PIN     (GPIO_PIN(0, 7))
```
or via the `CFLAGS` variable in the make command.
```
CFLAGS="-DCCS811_PARAM_INT_PIN=GPIO_PIN\(0,7\)" make -C tests/driver_ccs811_full BOARD=...
```
