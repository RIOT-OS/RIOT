/**
@defgroup    boards_dwm1001 Decawave DWM1001
@ingroup     boards
@brief       Support for the Decawave DWM1001 development board

## Overview

The [Devawave DWM1001 development board](https://www.decawave.com/product/dwm1001-development-board/)
includes a DWM1001 module which is based on a Nordic nRF51832 microcontroller.
This microcontroller is an ARM Cortex-M4 with 64KB of RAM and 512KB of flash
memory. It also provides in integrated BLE radio.
The DWM1001 module also contains an UWB transceiver connected to the
microcontroller via SPI (not supported yet).

## Flash the board

This board can be flashed using OpenOCD or JLink. Jlink is the default programmer.
To program this board, plug it to your computer via USB and run the following
command:

```
make BOARD=dwm1001 -C examples/hello-world flash
```

To program the board with OpenOCD, use:

```
PROGRAMMER=openocd make BOARD=dwm1001 -C examples/hello-world flash
```

## Accessing STDIO via UART

The STDIO is directly accessible via the USB port. On a Linux host, it's
generally mapped to `/dev/ttyACM0`.

Use the `term` target to connect to the board serial port<br/>
```
make BOARD=dwm1001 -C examples/hello-world term
```
 */
