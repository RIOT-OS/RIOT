@defgroup    boards_arduino-mkrwan1300 Arduino MKR WAN 1300
@ingroup     boards
@brief       Support for the Arduino MKR WAN 1300 board.

### General information

The [Arduino MKR WAN 1300](https://store.arduino.cc/mkr-wan-1300) board is
a learning and development board that provides LoRa connectivity and is
powered by an Atmel SAMD21 microcontroller.

### Pinout

<img src="https://www.arduino.cc/en/uploads/Main/MKR1000_pinout.png"
     alt="Arduino MKR WAN 1300 pinout" style="height:800px;"/>

### Flash the board

Use `BOARD=arduino-mkrwan1300` with the `make` command.<br/>
Example with `hello-world` application:
```
     make BOARD=arduino-mkrwan1300 -C examples/basic/hello-world flash
```

@note     If the application crashes, automatic reflashing via USB, as explained
          above won't be possible. In this case, the board must be set in
          bootloader mode by double tapping the reset button before running the
          flash command.

### Accessing STDIO via UART

STDIO of RIOT is directly available over the USB port.

The `TERM_DELAY` environment variable can be used to add a delay (in second)
before opening the serial terminal. The default value is 2s which should be
enough in most of the situation.
