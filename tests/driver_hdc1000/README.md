# About
This is a manual test application for the HDC1000 driver.

# Usage
This test application will initialize the HDC1000 sensor with the following parameters:
 - Temperature and humidity are acquired in sequence
 - Temperature measurement resolution 14 bit
 - Humidity measurement resolution 14 bit

After initialization, the sensor reads the temperature and humidity values every 1s
and prints them to STDOUT.
