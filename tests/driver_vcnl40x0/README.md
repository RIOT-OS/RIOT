## About
This is a test application for the VCNL40X0 proximity and ambient light sensor.

## Usage

After initialization, every 2 seconds, the application:
* reads the proximity (cts);
* reads the ambient light (cts);
* reads the illuminance (computed from ambient light by dividing it by 4);
* those values are printed to STDOUT.
