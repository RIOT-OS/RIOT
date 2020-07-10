# About

This is a manual test application for the L3GD20H gyroscope driver.

# Usage

This test application will initialize the gyroscope with the following parameters:

 - Sampling Rate: 50 Hz
 - Bandwidth:     16 Hz
 - Scale:         245 dps

The application is configurable which allows to try most features of the L3GD20H
gyroscope driver.

The default configuration uses the I2C interface (module ```l3gd20h_i2c```) of the sensor and the ```l3gd20h``` base module to initialize the sensor and to fetch data periodically every 10 ms.

With ```l3gd20h_spi``` module, the SPI interface of the sensor can be used. Using ```l3gd20h_full```, module the LPF and HPF filters are configured and used.

Different functions of the driver can be used with the following defines:

- ```USE_L3GD20H_INT_DATA_READY``` - Instead of polling the sensor, data-ready interrupt (```INT2/DRDY```) is used to read the data.
- ```USE_L3GD20H_INT_EVENT``` - Event detection is configured which triggers ```INT1``` interrupts when angular rate of any axis exceeds a configured threshold.
- ```USE_L3GD20H_FIFO``` - Instead of fetching individual data samples, the FIFO of the sensor is activated. Depending on the definition of ```USE_L3GD20H_INT_DATA_READY```, the FIFO is used either with polling or in conjunction with FIFO interrupts to read all data samples with a burst access.

**Please note:** To use these defines, module ```l3gd20h_full``` is required.

To use the mentioned modules and defines, the make command can be used, for example:
```
USEMODULE="l3gd20h_full l3gd20h_spi" \
CFLAGS="-DUSE_L3GD20H_INT_DATA_READY -DL3GD20H_PARAM_CS=GPIO_PIN\(0,8\)" \
make BOARD=... -C tests/driver_l3gd20h
```
In this example, using ```USE_L3GD20H_INT_DATA_READY``` requires module ```l3gd20h_full```. Furthermore, SPI is used and the default SPI cs pin is overriden.
