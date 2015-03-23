# About
This is a manual test application for the INA220 current and power monitor driver.

# Usage
This test application will initialize the sensor with the following parameters:
 - ADC resolution: 12 bit
 - Sampling time: 532 us
 - Calibration register: 4096

After initialization, the sensor reads the measurement values every 100ms
and prints them to the STDOUT.
