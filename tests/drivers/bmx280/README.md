## About
This is a test application for the BME280 and BMP280 sensors. Both can measure
pressure and temperature. The BME280 can also measure relative humidity.

## Usage
The application will initialize the BME280/BMP280 device and display its
calibration coefficients. More information can be found on the
[Bosch BME280 website][1], on the [Bosch BMP280 website][2],
in the [BME280 datasheet][3] and in the [BMP280 datasheet][4].

Notice that it is necessary to first read the temperature even if only one
of the other values (humidity or pressure) is needed. This is described in
the above mentioned datasheet.

After initialization, every 2 seconds, the application:
* reads and displays the temperature (d°C);
* reads and displays the pressure (Pa);
* reads and displays the humidity (%rH) (only with bme280);

## Overruling default parameters

If your device is at a different I2C address than the default (0x77) you
can build the test as follows:

    export CFLAGS=-DBMX280_PARAM_I2C_ADDR=0x76
    BOARD=sodaq-autonomo make -C tests/driver_bmx280

By default, the test application is built to use the bme280 module, to build it for
the bmp280, add `DRIVER=bmp280` to the previous command:


    $ DRIVER=bmp280 BOARD=sodaq-autonomo make -C tests/driver_bmx280


For more information, see the datasheets:
[1]: https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/
[2]: https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/pressure-sensors-bmp280-1.html
[3]: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
[4]: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf
