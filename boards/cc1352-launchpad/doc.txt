/**
@defgroup        boards_cc1352_launchpad TI CC1352 LaunchPad
@ingroup         boards
@brief           Texas Instruments SimpleLink(TM) CC1352 Wireless MCU LaunchPad(TM) Kit

## Overview

The [LAUNCHXL-CC1352R1](https://www.ti.com/tool/LAUNCHXL-CC1352R1) is a Texas
Instrument's development kit for the CC1352R1 SoC MCU which combines a
Cortex-M4F microcontroller alongside a dedicated Cortex-M0 to control radio.

## Hardware

![LAUNCHXL-CC1352R1](https://www.ti.com/diagrams/launchxl-cc1352r1_launchxl-cc1352r1-angled.jpg)

| MCU               | CC1352R1              |
|:----------------- |:--------------------- |
| Family            | ARM Cortex-M4F        |
| Vendor            | Texas Instruments     |
| RAM               | 80KiB                 |
| Flash             | 352KiB                |
| Frequency         | 48MHz                 |
| FPU               | yes                   |
| Timers            | 4                     |
| ADCs              | 1x 12-bit (channels)  |
| UARTs             | 2                     |
| SPIs              | 2                     |
| I2Cs              | 1                     |
| Vcc               | 1.8V - 3.8V           |
| Datasheet         | [Datasheet](http://www.ti.com/lit/ds/symlink/cc1352r.pdf) (pdf file) |
| Reference Manual  | [Reference Manual](http://www.ti.com/lit/ug/swcu185d/swcu185d.pdf) |

## Board pinout

The [LAUNCHXL-CC1352R1 Quick Start Guide](https://www.ti.com/lit/ml/swru525e/swru525e.pdf)
provides the default pinout for the board.

## Flashing the Device

Flashing RIOT is quite straight forward. The board comes with an XDS110 on-board
debug probe that provides programming, flashing and debugging capabilities
through the USB Micro-USB connector. Once either TI Uniflash or OpenOCD are
installed just connect the board using the Micro-USB port to your computer and
type:

```
make flash BOARD=cc1352-launchpad
```

To use OpenOCD instead of uniflash we need to set the `PROGRAMMER` environment
variable, this is to enable OpenOCD instead of Uniflash.

```
export PROGRAMMER=openocd
```

Now we can just do `make flash` and `make debug`, this all using OpenOCD.

## Accessing RIOT shell

Default RIOT shell access utilize XDS110 debug probe integrated with launchpad board.
It provides virtual serials via USB interface - for connecting to RIOT shell, use
the first one.

If a physical connection to UART is needed, disconnect jumpers RXD and TXD joining
cc1352 microcontroller with XDS110 and connect UART to pin RXD/DIO12 and TXD/DIO13.

The default baud rate is 115 200 - in both connection types.

@warning Launchpad cc1352 board is not 5V tolerant. Use voltage divider or logic
level shifter when connecting to 5V UART.

## More information

For detailed information about CC1352R1 MCUs as well as configuring, compiling
RIOT and installation of flashing tools for CC1352R1 boards,
see \ref cc26xx_cc13xx_riot.

*/
