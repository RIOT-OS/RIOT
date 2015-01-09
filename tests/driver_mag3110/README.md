# About
This is a manual test application for the MAG3110 magnetometer driver.

# Usage
This test application will initialize the MAG3110 with the following parameters:
 - output rate set to 1.25 Hz
 - over sample ratio set to 128

After initialization, the sensor reads the x-, y-, z-axis values every 1 s
and prints them to STDOUT.
