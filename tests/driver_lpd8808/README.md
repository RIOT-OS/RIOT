# About
This test application is made for verification of the LPD8808 LED strip driver.

# Usage
Connect a LPD8808 based LED strip to a board of your choice, build, and flash
this application. You should see a light running up and down your strip,
slowly changing it's color.

You might need to adjust the default parameters (number of LEDs on the strip and
pin configuration). You can do this simply by pre-setting the `CFLAGS`
environment variable, e.g.:
```
$ CFLAGS="-DLPD8808_PARAM_LED_CNT=78"" make all
```
