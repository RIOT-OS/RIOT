# About
This test application is created for testing DS1307 driver.
It uses the default device parameters as specified in
`drivers/ds1307/include/ds1307_params.h`. To override these setting, you
can simply do this by defining these parameters as compiler flags while building,
e.g.:
```
$ CFLAGS="-DDS1307_PARAM_I2C=I2C_DEV(1)"" make all
```

# Usage
Simply flash this example to your board and after a while you will see a number
of points and then `OK (x tests)` where `x` corresponds to the number of tests.

The output might take a while.
