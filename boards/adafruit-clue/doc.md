@defgroup    boards_adafruit-clue Adafruit Clue
@ingroup     boards
@brief       Support for the Adafruit Clue

### General information

The [Adafruit Clue](https://www.adafruit.com/clue) board
is an opensource, micro development kit using the nRF52840 SoC.
This board provides 802.15.4 and BLE connectivity.

<img src="https://raw.githubusercontent.com/adafruit/Adafruit-CLUE-PCB/master/assets/4500.jpg"
     alt="Adafruit Clue" style="height:800px;"/>

### Schematics

The board detailed description and schematic is available
[here](https://github.com/adafruit/Adafruit-CLUE-PCB).

### Flash the board

The board is flashed using its on-board
[bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader).
More information about how to work with this bootloader can be found in the
@ref boards_common_adafruit-nrf52-bootloader section.

### Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>
```
    make BOARD=adafruit-clue -C examples/basic/hello-world term
```
