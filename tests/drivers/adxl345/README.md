# About
This is a test application for the ADXL345 accelerometer.

# Datasheet
http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf

# Usage
This test application will initialize the ADXL345 sensor with the following parameters:
 - I2C device address set to 0x53
 - full scale parameter set to +/-16 g
 - 200 Hz output data-rate

See RIOT/drivers/adxl345_params.h for the default configuration.

After initialization, the sensor reads the x-, y-, z-axis values every 100ms
and prints them to STDOUT.
