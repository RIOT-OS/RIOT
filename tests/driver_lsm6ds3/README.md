# About
Test application for the LSM6DS3 sensor, with 3D accelerometer and 3D gyroscope.

# Usage
This test application will be initialize with the following parameters:

General:
 - Temperature sensor: Enabled

Accelerometer:
 - Sampling Rate: 13Hz
 - Scale:         2G

Gyroscope:
 - Sampling Rate: 13Hz
 - Scale:         245DPS

After initialization, the sensor reads the temperature, accelerometer and gyroscope values every 500ms and prints them to the STDOUT.
