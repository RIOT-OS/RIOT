# About
This is a manual test application for the SHT2x humidity/temperature sensor driver.

# Usage
This test application will initialize the SHT2x sensor with the following parameters:
 - default device type: SHT25
 - 14-bit temperature resolution
 - 12-bit humidity resolution

To change these parameters, you can override them by setting their corresponding
defines in your build environment, e.g.
```bash
CFLAGS=-DSHT2X_PARAM_RESOLUTION=SHT2x_RES_8_12BIT make ...
```
See RIOT/drivers/sht2x_params.h for the default configuration.

After initialization, the sensor reads the temperature and humidity values every
one second and prints them to STDOUT.
