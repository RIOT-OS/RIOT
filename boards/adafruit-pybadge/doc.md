@defgroup    boards_adafruit-pybadge Adafruit PyBadge
@ingroup     boards
@brief       Support for the Adafruit PyBadge board.

### General information

The [Adafruit PyBadge](https://www.adafruit.com/product/4200) is a
learning and development board that provides 5 Neopixel LEDs, an accelerometer,
a light sensor, a speaker, several buttons and an LCD screen.
It is powered by an Atmel SAMD51 microcontroller.

The display, RGB LEDs, buttons and accelerometer of this board are currently not supported.

The [Adafruit PyBadge LC](https://www.adafruit.com/product/3939) and
[Adafruit EdgeBadge](https://www.adafruit.com/product/4400)) are
variants of the Pybadge with small differences:
- EdgeBadge provides an extra microphone (not supported)
- PyBadgeLC only have one Neopixel LED (instead of 5)
Both are compatible with the PyBadge RIOT port and the current port can easily
be adapted/extended with their respective differences.

### Flash the board

Connect the board via USB and use `BOARD=adafruit-pybadge` with the `make` command;
this uses the Arduino style bootloader preprogrammed on the board.<br/>
Example with `hello-world` application:
```
     make BOARD=adafruit-pybadge -C examples/basic/hello-world flash
```

In case of a crash of the firmware, one has to manually reset the board in
bootloader mode to be able to re-flash it again: simply double tap the reset
button before running the flash command provided above.

### Accessing STDIO via UART

STDIO of RIOT is directly available over the USB port.
