@defgroup    boards_adafruit-itsybitsy-m4 Adafruit-Itsybitsy-M4
@ingroup     boards
@brief       Support for the Adafruit-Itsybitsy-M4

### General information

![Adafruit-Itsybitsy-M4 compared to a quarter dollar](https://cdn-learn.adafruit.com/assets/assets/000/055/465/large1024/adafruit_products_3800_quarter_ORIG_2018_06.jpg?1529192175)

This is a small formfactor (only 1.4\" long by 0.7\" wide) SAM D51 board made by Adafruit.

The board features one red LED (LD1), one DotStar / APA102 RGB LED, a reset button as well as
21 configurable external pins(6 of which can be analog in).

### Links

- [Overview](https://learn.adafruit.com/introducing-adafruit-itsybitsy-m4?view=all)
- [Schematics](https://learn.adafruit.com/assets/55481)

### Flash the board

The board is flashed using its on-board [boot loader](https://github.com/adafruit/uf2-samdx1).

The process is automated in the usual `make flash` target.

If RIOT is already running on the board, it will automatically reset the CPU and enter
the bootloader.
If some other firmware is running or RIOT crashed, you need to enter the bootloader
manually by double tapping the board's reset button.

Readiness of the bootloader is indicated by LD1 pulsing in red.

### Accessing STDIO

The usual way to obtain a console on this board is using an emulated USB serial port.
