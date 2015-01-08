# About
This is a manual test application for the MMA8652 accelerometer driver.

# Usage
This test application will initialize the MMA8652 sensor with the following parameters:
 - full scale parameter set to +/-2 g
 - 6.25 Hz output data-rate

After initialization, the sensor reads the x-, y-, z-axis values every 1 s
and prints them to STDOUT.
