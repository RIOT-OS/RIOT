# Test application for ST VL6180X Ranging and Ambient Light Sensing (ALS) module

## About

The test application demonstrates the use of different functions of
the ST VL6180X sensor driver depending on used pseudomodules:

Module Name        | Functionality used
:------------------|:-------------------------------------------
`vl6180x_rng`      | Periodic range measurements enabled
`vl6180x_als`      | Periodic ambient light sensing (ALS) enabled
`vl6180x_irq`      | Data ready interrupt enabled for range and ALS measurements
`vl6180x_config`   | Event interrupt enabled and configured for range measurements
`vl6180x_shutdown` | Power-down and power-up functions used

## Usage

To compile and execute the test application, use command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make BOARD=... -C tests/drivers/vl6180x flash term
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By default, the `vl6180x_rng` and `vl6180x_als` modules are enabled.
This is, the standard variant of the test application performs periodic
range and ALS measurements.

To use data ready interrupts for range and ALS measurements instead of
polling for new data, the `vl6180x_irq` module has to be used:

The MCU GPIO pin connected with the interrupt signal GPIO1 of the sensor
has to be defined by the configuration parameter `VL6180X_PARAM_PIN_INT`,
for example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS="-DVL6180X_PARAM_INT_PIN=\(GPIO_PIN\(0,5\)\)" \
USEMODULE=vl6180x_irq \
make BOARD=... -C tests/drivers/vl6180x flash term
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the module `vl6180x_config` is used additionally, the event interrupt
is configured and used for range measurements:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS="-DVL6180X_PARAM_INT_PIN=\(GPIO_PIN\(0,5\)\)" \
USEMODULE='vl6180x_irq vl6180x_config'\
make BOARD=... -C tests/drivers/vl6180x flash term
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To test the power-down and power-up functionality, the `vl6180x_shutdown`
module has to be used. The MCU GPIO pin connected with the signal GPIO0/CE
of the sensor has to be defined by configuration parameter
`VL6180X_PARAM_PIN_SHUTDOWN`, for example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS="-VL6180X_PARAM_SHUTDOWN_PIN=\(GPIO_PIN\(0,6\)\)" \
USEMODULE=vl6180x_shutdown \
make BOARD=... -C tests/drivers/vl6180x flash term
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
