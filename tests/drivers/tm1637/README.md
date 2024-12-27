# Test Application for the TM1637 4 digit 7-segment display.

## About

This is a simple test application for the TM1637 driver.

## Details

This test application will initialize the TM1637 driver with the configuration
as specified in the default `tm1637_params.h` file. The 4 pins of the display need to
be connected in this way:
- The clk pin connects to GPIO 0
- The dio pin connects to GPIO 1
- The VCC pin connects to either 5V or 3.3V
- The GND pin connects to GND

## Expected result
The displays switches between different configurations, all digits and all brightness levels.