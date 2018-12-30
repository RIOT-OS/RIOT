# About

This is a manual test application for the LPSXXX family of pressure sensors
driver. This driver can be used with LPS331AP and LPS25HB.

Default driver is `lps331ap`. To use the LPS25HB driver, set the `DRIVER` when
building the application:

    DRIVER=lps25hb make BOARD=<your board>

# Usage

This test application will initialize the pressure sensor with the following
parameters:

 - Sampling Rate: 7Hz

After initialization, the sensor reads the pressure and temperature values
every 250ms and prints them to the STDOUT.
