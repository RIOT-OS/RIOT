/**
@defgroup    boards_arduino-nano-33-iot Arduino Nano 33 IoT
@ingroup     boards
@brief       Support for the Arduino Nano 33 IoT board.

### General information

The [Arduino Nano 33 IoT](https://store.arduino.cc/arduino-nano-33-iot) board is
a learning and development board that provides Sigfox connectivity and is
powered by an Atmel SAMD21 microcontroller.

### Flash the board

Use `BOARD=arduino-nano-33-iot` with the `make` command.<br/>
Example with `hello-world` application:
```
     make BOARD=arduino-nano-33-iot -C examples/hello-world flash
```

### Accessing STDIO via UART

STDIO of RIOT is directly available over the USB port.

The `TERM_DELAY` environment variable can be used to add a delay (in second)
before opening the serial terminal. The default value is 2s which should be
enough in most of the situation.
 */
