# About
This test application is created for testing/demonstrating the BH1900NUX driver.
It uses the default device parameters as specified in
`drivers/bh1900nux/include/bh1900nux.h`. To override these setting, you
can simply do this by defining these parameters as compiler flags while uilding,
e.g.:
```
$ CFLAGS="-DBH1900NUX_PARAM_I2C=I2C_DEV(1)"" make all
```

# Usage
Simply flash this example to your board and it will read the sensor value 5
times per second and print the sampled value to STDIO. You can verify the values
by holding your finger on the sensor to heat it up, or blow on it to cool it down.
