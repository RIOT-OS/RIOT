## About

This is a test application for the Maxim
[DS18B20](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf)
1-Wire temperature sensor.

## Usage

The application will initialize the DS18B20 sensor, read
the temperature and print the measurement to STDOUT every two seconds.

The default GPIO pin is (0,0), which equates for example to PA0 for STM32s.
If you connected sensor to other pin you should set `DS18_PARAM_PIN` accordingly.
Guide showing how to change default driver configuration is provided by
[link](https://guide.riot-os.org/advanced_tutorials/device_drivers/#default-device-configuration) .
