@defgroup    boards_adafruit-itsybitsy-nrf52 Adafruit-Itsybitsy-nRF52
@ingroup     boards
@brief       Support for the Adafruit-Itsybitsy-nRF52

### General information

This is a small formfactor (only 1.4\" long by 0.7\" wide) nRF52840 board made by Adafruit.

The board features one red LED (LD1), one DotStar / APA102 RGB LED, a user (SW1), a
reset button as well as 21 configurable external pins(6 of which can be analog in).

### Links

- [Overview](https://learn.adafruit.com/adafruit-itsybitsy-nrf52840-express?view=all)
- [Pinouts](https://learn.adafruit.com/adafruit-itsybitsy-nrf52840-express/pinouts)

### Flash the board

The board is flashed using its on-board
[bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader).
More information about how to work with this bootloader can be found in the
@ref boards_common_adafruit-nrf52-bootloader section.

### Accessing STDIO

The usual way to obtain a console on this board is using an emulated USB serial port.
