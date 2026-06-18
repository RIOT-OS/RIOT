# LIS331DLH accelerometer test

This test application initializes a LIS331DLH 3-axis accelerometer on the
configured I2C bus and periodically prints acceleration values in milli-g.

The default configuration uses `I2C_DEV(0)` and I2C address `0x18`. These can
be overridden from the command line, for example:

```
CFLAGS='-DLIS331DLH_PARAM_I2C=I2C_DEV(0) -DLIS331DLH_PARAM_ADDR=LIS331DLH_I2C_ADDR_1' make BOARD=<board>
```

Expected output with a connected device:

```
LIS331DLH accelerometer test
x: ... mg, y: ... mg, z: ... mg
```
