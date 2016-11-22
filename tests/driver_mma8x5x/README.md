# About
This is a manual test application for the MMA8x5x accelerometer driver.

# Usage
This test application will initialize the MMA8x5x sensor with the following parameters:
 - default device type: MMA8652
 - full scale parameter set to +/-2 g
 - 200 Hz output data-rate

To change these parameters, you can override them by setting their corresponding
defines in your build environment, e.g.
```bash
CFLAGS=-DMMA8X5X_PARAM_TYPE=MMA8X5X_TYPE_MMA8451 make ...
```
See RIOT/drivers/mma8x5x_params.h for the default configuration.

After initialization, the sensor reads the x-, y-, z-axis values every 100ms
and prints them to STDOUT.
