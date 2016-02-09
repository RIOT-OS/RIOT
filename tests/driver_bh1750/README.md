# About
This test application is created for testing/demonstrating the BH1750FVI driver.
It uses the default device parameters as specified in
`drivers/bh1750fvi/include/bh1750fvi_params.h`. To override these setting, you
can simply do this by defining these parameters as compiler flags while uilding,
e.g.:
```
$ CFLAGS="-DBH1750FVI_PARAM_I2C=I2C_DEV(1)"" make all
```

# Usage
Simply flash this example to your board and it will read the sensor value 5
times per second and print the sampled value to STDIO. You can verify the values
by simply covering the sensor or holding it in front of a bright light and see
the change in the sensor readings accordingly.
