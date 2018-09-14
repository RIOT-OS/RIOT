# About
This is a manual test application for the ISL29020 light sensor driver.

# Usage
This test application will initialize the list sensor with the following parameters:
 - Mode: Ambient light measurement
 - Range: 16000LUX

After initialization, the sensor value is read every 250ms and printed to the STDOUT.

To verify the seen value you can hold the sensor into a bright light or shield the sensor to
see the value changing.
