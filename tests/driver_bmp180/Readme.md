## About
This is a test application for the BMP180 Pressure and Temperature sensor.

## Usage
The application will initialize the BMP180 sensor and display its calibration
coefficients. Please see section 3.4 of the
[datasheet](https://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf) for
more information.

After initialization, every 2 seconds, the application:
* reads the temperature (d°C);
* reads the pressure (Pa);
* computes the pressure at sea level based on the TEST_ALTITUDE variable;
* computes the altitude based on the pressure at sea level calculated above;
* those values are then printed to STDOUT.
