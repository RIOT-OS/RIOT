#About
Testing application for the SHTCX faimly of temperature and humidity sensors.
This driver can be used with the SHTC1 and SHTC3 sensors.

Default driver is "shtc1". To use the SHTC3 driver, set the "DRIVER" when building the application:

    DRIVER=shtc3 make BOARD=<your board>

#Usage
Just build it using the `make BOARD=??? flash` command in the `tests/driver_shtcx` folder. Temperature and humidity values should be printed in the terminal every 2s
