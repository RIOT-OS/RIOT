/**
@defgroup    boards_lsn50 Dragino LSN50 LoRa Sensor Node
@ingroup     boards
@brief       Support for the Dragino LSN50 LoRa Sensor Node

### Introduction

This board is a waterproof board with a LoRa SX1276 radio.

![LSN50](https://wiki.dragino.com/images/thumb/e/e9/Introdution.png/600px-Introdution.png)

Documentation of the board is available
[here](https://wiki.dragino.com/index.php?title=Lora_Sensor_Node-LSN50).

More documentation is available
[here](http://wiki.dragino.com/index.php?title=Lora_Sensor_Node-LSN50#Resource).

The datasheet can be downloaded
[here](https://www.dragino.com/downloads/index.php?dir=datasheet/EN/&file=Datasheet_LoRaSensorNode.pdf).

Detailed schematics are available on GitHub:
- Board schematics are [here](https://github.com/dragino/Lora/tree/master/LSN50)
- Radio connection schematics are [here](https://github.com/dragino/Lora/tree/master/LoRaST)

### Flashing the board

To flash the board, use an external ST-Link programmer/debugger, plugged on
available SWD pins: PA13 (SWDIO), PA14 (SWCLK) and NRST (this pin is not
exposed with v1.0).
Ensure SW1 is on `flash` position.
Then use the following command:

    make BOARD=lsn50 -C examples/hello-world flash

On the v1.0 version of the board, no NRST pin is exposed so one has to press the
reset button during flash and release it when OpenOCD prints `adapter speed: 240 kHz`
the first time.
The reset button must also be pressed a second time after flashing to start the new
application.

### STDIO

STDIO is connected to pins PA9 (TX) and PA10 (RX) so an USB to UART adapter is
required. Use the `term` target to open a terminal:

    make BOARD=lsn50 -C examples/hello-world term

If an external ST-Link adapter is used, RX and TX pins can be directly connected
to it. In this case, STDIO is available on /dev/ttyACMx (Linux case).

 */
