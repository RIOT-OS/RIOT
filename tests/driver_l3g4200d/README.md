# About
This is a manual test application for the L3G4200D gyroscope driver.

# Usage
This test application will initialize the pressure sensor with the following parameters:
 - Sampling Rate: 100Hz
 - Bandwidth:     25Hz
 - Scale:         500DPS

After initialization, the sensor reads the angular speed values every 10ms
and prints them to the STDOUT.
