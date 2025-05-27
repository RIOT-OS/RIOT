# MAX31865 driver test

## About

This is a test application for the
[MAX31865](https://www.analog.com/en/products/max31865.html)
driver, a RTD-to-digital converter, tailored for Pt100 and Pt1000
sensing elements.

## Usage

This test application will initialize a MAX31865 device to output
the temperature every second.

The driver uses fixed-point arithmetic for all conversions.
To do so, it needs a lookup table (LUT).
A default one is provided for a Pt100 and a 330&Omega; reference resistor.
The user can generate one for different resistor values with the
`dist/genlut.py` script.
