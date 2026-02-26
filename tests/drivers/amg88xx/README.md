# AMG88xx Driver Test

## Introduction
This test application tests the driver for the Panasonic AMG88xx family of
infrared (thermal) array sensors. The following sensors are supported:

* AMG8831
* AMG8832
* AMG8833
* AMG8834
* AMG8851
* AMG8852
* AMG8853
* AMG8854

## Usage
Use the `help` command in the shell to see the available commands and their
usage.

The interrupt feature is only available if the `periph_gpio_irq` feature
is available and if the interrupt pin is properly configured (see
`amg88xx_params`).

### Display mode
Adding a display to the test application is optional, but it can be a useful
way to visualize the thermal sensor data. The display is updated every time a
new frame is read from the sensor (either via the `read`, `poll` or `interrupt`
command).

To enable this, ensure that the `disp_dev` and a compatible display driver
(e.g. `st77xx`) are included in the test application. See the `Makefile` for
details. Then, the `display` command can be used to toggle the display on and
off.

Tip: set `grid off`, `fps 10` and then use `poll 100` to update the display as
fast as possible.

## Expected result
After successful initialization, a shell is available to interact with the
sensor. Most notably, the following commands are available to read data from
the sensor:

* The `read` command reads a single 8x8 thermal frame and prints the values
  in a grid.
* The `poll [interval]` command does this continuously at the given interval
  in milliseconds (default: 1000 ms). Use `stop` to cancel.
* The `temp` command reads the on-chip thermistor temperature.
