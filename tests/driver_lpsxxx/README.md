# About

This is a manual test application for the LPSXXX family of pressure sensors
driver. This driver can be used with LPS331AP, LPS25HB and LPS22HB.

Default driver is `lps331ap`. To use the LPS25HB driver, set the `DRIVER` when
building the application:

    DRIVER=lps25hb make BOARD=<your board>

This driver can be used with any combination of SPI and/or I2C buses.

For support of only I2C use:

DRIVER=<lps331ap or lps25hb or lps22hb> CONFIG_LPSXXX_I2C=1 \
make BOARD=<your board>

For support of only SPI use:

DRIVER=<lps331ap or lps25hb or lps22hb> CONFIG_LPSXXX_SPI=1 \
make BOARD=<your board>

For support of I2C and SPI use:

DRIVER=<lps331ap or lps25hb or lps22hb> CONFIG_LPSXXX_SPI=1 \
CONFIG_LPSXXX_SPI=1 make BOARD=<your board>

An example of a case where I2C and SPI needs to be used, would be for
a board with at least one LPSXXX, connected to an I2C bus, and at least
one LPSXXX, connected to an SPI bus.

# Usage

This test application will initialize the pressure sensor with a sampling rate
of 7Hz (25Hz with lps22hb).

After initialization, the sensor reads the pressure and temperature values
every 250ms and prints them to the STDOUT.
