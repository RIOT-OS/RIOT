# About
This is a manual test application for the INA219/INA220 current and power
monitor driver.

# Usage
This test application will initialize the sensor with the default parameters
in @ref ina2xx_params.h

After initialization, the application will print a table of the devices
measurement data. If the I2C interface is fast enough, one row corresponds to
one measurement of the device. (The sensor indicates when a new measurement is
available via flags, this flags are read until the measurement is done. The
duration of one measurement depends on the sensor settings and is between
84µs and 68.1ms. Please note that blocking stdio might very well be the
bottleneck causing the test to miss measurements.)
