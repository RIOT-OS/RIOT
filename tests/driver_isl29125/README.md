# About
This is a manual test application for the ISL29125 light sensor driver.

# Usage
This test application will initialize the list sensor with the following parameters:
 - Mode: All modes are tested once, then RGB mode is used continuously
 - Range: 16000 lux
 - Resolution: 16 bit

After initialization, the sensor value is read every 250ms and printed to the stdout.

Expose the sensor to varying light sources to see changing readouts.
