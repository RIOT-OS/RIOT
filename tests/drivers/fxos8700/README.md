# About
This is a manual test application for the FXOS8700 driver.

# Usage
This test application will initialize the FXOS8700 sensor with the configuration
as specified in the default `fxos8700_params.h` file.

If you want to use this test application with different parameters, you can
simply override the default FXOS8700_PARAMS.

After initialization, the sensor reads the 3-axis acceleration and 3-axis
magnetic field values every second and prints them to STDOUT.
