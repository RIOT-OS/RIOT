# About

This is a manual test application for the LPSXXX family of pressure sensors
driver. This driver can be used with LPS331AP, LPS25HB, LPS22HB and LPS22HH.

Default driver is `lps331ap`. To use the LPS25HB driver, set the `DRIVER` when
building the application:

    DRIVER=lps25hb make BOARD=<your board>

# Usage

This test application will initialize the pressure sensor with a sampling rate
of 7Hz (25Hz with lps22hb and lps22hh).

After initialization, the sensor reads the pressure and temperature values
every 250ms and prints them to the STDOUT.
