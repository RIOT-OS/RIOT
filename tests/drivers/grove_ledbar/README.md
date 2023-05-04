# About

This is a test application for the Seeed Studio Grove LED bar module [1].

# Details

This test application will initialize a Groove LED bar with the configuration
as specified in the `grove_ledbar_params.h` file. To connect the LED bar with
your board use the following pinout:

- Pin GND is connected directly to GND.
- Pin VCC is connected directly to VCC +3.3V, or 5V if your board allows that.
- Pin DCKI is the clock pin and is connected to a free GPIO
- Pin DI is the data pin and is connected to a free GPIO, too

for the 2 latter pins note the port and pin number and adapt the Makefile
accordingly.

The test application will light up the LED bar several times, running from 0%
up to 100% and down to 0% again.

[1]: https://www.seeedstudio.com/Grove-LED-Bar-v2.0-p-2474.html
