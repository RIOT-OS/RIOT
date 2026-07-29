# MAX31343 Test Application

This test can be used to test and evaluate the Analog Devices MAX31343
Real-Time Clock.

Currently the driver does not support interrupts natively and the test
application does not provide interrupt support either. Only the I2C bus to
which the RTC is connected has to be selected in the parameters.

The test application provides shell commands that can be used to configure
and control the RTC.
