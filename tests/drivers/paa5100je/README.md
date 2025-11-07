# About
This is a manual test application for the PAA5100JE/PMW3901 optical flow sensor
driver. It reads motion data from the sensor and prints it to the console.

To run the test, connect the PAA5100JE/PMW3901 sensor to your MCU.
The sensor uses SPI for communication, so make sure to connect the SPI pins
correctly. You will also need to connect the chip select (CS) pin and power
the sensor with 3.3V to 5.0V and GND.

Before running the test, you may need to adjust the pin configuration and select
the right version of the sensor (PAA5100JE or PMW3901).
