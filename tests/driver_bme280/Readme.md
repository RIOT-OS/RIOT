## About
This is a test application for the BME280 Pressure, Temperature and
Humidity sensor.

## Usage
The application will initialize the BME280 device and display its
calibration coefficients. More information can be found on the
[Bosch website][1]
And in this [BST-BME280_DS001-11 datasheet] [2]

Notice that it is necessary to first read the temperature even if only one
of the other values (humidity or pressure) is needed. This is described in
the above mentioned datasheet.

After initialization, every 2 seconds, the application:
* reads and displays the temperature (d°C);
* reads and displays the pressure (Pa);
* reads and displays the humidity (%rH);

## Overruling default parameters

If your device is at a different I2C address than the default (0x77) you
can build the test as follows:

    export CFLAGS=-DBME280_PARAM_I2C_ADDR=0x76
    make -C tests/driver_bme280 BOARD=sodaq-autonomo

[1]: http://www.bosch-sensortec.com/en/bst/products/all_products/bme280

[2]: https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
