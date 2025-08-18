# About

This is a test application for the HD44780 LCD driver. This display comes with
many Arduino starter kits under the name of LCM1602C, and typically has 16x2
columns and rows.

This examples covers the parallel mode of the HD44780 driver. For the I2C mode,
please refer to `tests/drivers/hd44780_i2c`.

# Details

This test application will initialize the HD44780 driver with the configuration
as specified in the default `hd44780_params.h` file. To connect the display with
your board use the following minimal pinout for the LCD (i.e., Arduino here):

- Pin 1 is connected directly to GND.
- Pin 2 is connected directly to VCC +5V.
- Pin 3 is used to set LCD contrast, for max use +5V or a 10k potentiometer.
- Pin 4 (RS or "register select") is connected to pin 2 on the Arduino
- Pin 5 (RW or "read/write") is connected directly to GND, i.e., unused.
  Also note: if you connect RW to your board that the LCD is driven by 5V, while
  many boards internally run at 3.3V - so this could fry the board :/
- Pin 6 (EN or "enable") is connected to pin 3 on the Arduino.
- Pins 7 - 10: Not connected.
- Pin 11 on the LCD is connected to pin 4 on the Arduino.
- Pin 12 on the LCD is connected to pin 5 on the Arduino.
- Pin 13 on the LCD is connected to pin 6 on the Arduino.
- Pin 14 on the LCD is connected to pin 7 on the Arduino.
- Pin 15 is connected to one end of a 1k resistor, and its other end to VCC +5V.
- Pin 16 is connected directly to GND.
