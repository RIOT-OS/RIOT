/**
@defgroup    boards_teensy31 Teensy3.1 & 3.2
@ingroup     boards
@brief       Support for the Teensy3.1 & 3.2

### General information

Teensy3.1 & 3.2 boards are development boards made by
[PJRC](https://www.pjrc.com/teensy/teensy31.html).

Teensy3.1 & 3.2 boards are built based on the Freescale MK20DX256VLH7
microcontroller. See [Datasheet](http://cache.freescale.com/files/32bit/doc/data_sheet/K20P64M72SF1.pdf).

### Pinout

<img src="https://www.pjrc.com/teensy/teensy32_front_pinout.png"
     alt="Teensy 3.2 front pinout" style="width:800px;"/>

<img src="https://www.pjrc.com/teensy/teensy32_back_pinout.png"
     alt="Teensy 3.2 back pinout" style="width:800px;"/>

### Flash the board

1. Put the board in bootloader mode by tapping the reset button.<br/>
   The board should remain in bootloader mode until flashed.

2. Use `BOARD=teensy31` with the `make` command. This works for Teensy 3.1 & 3.2<br/>
   Example with `hello-world` application:
```
     make BOARD=teensy31 -C examples/hello-world flash
```

### Accessing STDIO via UART

To access the STDIO of RIOT, a FTDI to USB converter needs to be plugged to
the RX/TX pins on the board (pins 0 and 1 of the board).
 */
