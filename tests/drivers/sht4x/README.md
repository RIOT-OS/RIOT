# SHT4x Test Application

This is a manual test application for the SHT4x sensor driver. It initializes the sensor, reads temperature and humidity periodically, and prints the values to standard output.

To build this test from the RIOT root:

    make tests/drivers/sht4x BOARD=<board>

