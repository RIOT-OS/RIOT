## About
This is a test application for the BME280 and BMP280 sensors. Both can measure
pressure and temperature. The BME280 can also measure relative humidity.

## Usage
The application will initialize the BME280/BMP280 device and display its
calibration coefficients. More information can be found on the
[Bosch website][1], in the [BST-BME280_DS001-11 datasheet] [2] and in the
[BST-BMP280-DS001-12  datasheet] [3].

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

    export CFLAGS=-DBME280_PARAM_I2C_ADDR=0x76
    BOARD=sodaq-autonomo make -C tests/driver_bmx280

By default, the test application is built to use the bme280 module, to build it for
the bmp280, add `DRIVER=bmp280` to the previous command:


    $ DRIVER=bmp280 BOARD=sodaq-autonomo make -C tests/driver_bmx280


For more information, see the datasheets:
[1]: http://www.bosch-sensortec.com/en/bst/products/all_products/bme280
[2]: https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
[3]: https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP280-DS001-12.pdf
