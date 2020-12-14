# About
This is a manual test application for the HDC20100 driver.

# Usage
This test application will initialize the HDC2010 sensor with the configuration
as specified in the default `hdc2010_params.h` file (i.e. 14-bit resolution).

If you want to use this test application with different parameters, you can
simply override the default settings on the command line like this:
```
CFLAGS="-DHDC20100_PARAM_I2C=I2C_DEV\(1\) -DHDC1000_PARAM_RES=HDC2010_11BIT" make
```

After initialization, the sensor reads the temperature and humidity values every
second and prints them to STDOUT.
