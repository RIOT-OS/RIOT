# MAX313xx Test Application

This test can be used to test and evaluate the Analog Devices MAX313xx
Real-Time Clocks.

The test application provides shell commands that can be used to configure
and control the RTC.

## Supported Devices and Implementation Status

Currently the driver does not support interrupts natively and the test
application does not provide interrupt support either. There is no support for
the User RAM. Only the I2C bus to
which the RTC is connected has to be selected in the parameters.

The driver has support for the MAX31331 and MAX31343, please select the variant
you have in the `Makefile` or specify it in the command line with the `DRIVER`
environment variable:
```sh
DRIVER=max31331 make -C tests/drivers/max313xx/ flash term

```
