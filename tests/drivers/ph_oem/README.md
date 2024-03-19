## About
This is a manual test application for the Atlas Scientific pH OEM sensor driver.

## Usage
This application will verify the correct operation of the pH OEM sensor by testing
all possible functionality, based on the pin configuration you use.

The enable and interrupt pin are undefined by default. If you want to use them
define them for example in the makefile (an example is already there).

The calibration test is disabled by default, so it will not reset your
previously done calibrations.

After initialization and successful tests, the sensor reads the pH value every
5 seconds and prints them to the STDOUT.
