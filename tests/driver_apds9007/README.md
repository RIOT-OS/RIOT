# About
This is a manual test application for the APDS9007 driver.

# Usage
This test application will initialize the APDS9007 sensor with the configuration
as specified in the default `apds9007_params.h` file.

If you want to use this test application with different parameters, you can
simply override the default APDS9007_PARAMS.

After initialization, the sensor reads the pulse counter values every second and
prints them to STDOUT.
