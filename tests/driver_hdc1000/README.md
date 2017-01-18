# About
This is a manual test application for the HDC1000 driver.

# Usage
This test application will initialize the HDC1000 sensor with the configuration
as specified in the default `hdc1000_params.h` file (i.e. 14-bit resolution).

If you want to use this test application with different parameters, you can
simply override the default settings on the command line like this:
```
CFLAGS="-DHDC1000_PARAM_I2C=I2C_DEV\(1\) -DHDC1000_PARAM_RES=HDC1000_11BIT" make
```

After initialization, the sensor reads the temperature and humidity values every
second and prints them to STDOUT.
