/**
@defgroup    boards_samr34-xpro Microchip SAM R34 Xplained Pro
@ingroup     boards
@brief       Support for the Microchip SAM R34 Xplained Pro and WLR089 Xplained Pro

## Overview

The `SAMR34 Xplained Pro` is a compact evaluation board by Microchip featuring a
SAMR34J18 SoC. The SoC includes a SAML21 ARM Cortex-M0+ micro-controller
bundled with Semtech's SX1276, a 868/915 MHz LoRa compatible radio.
For programming the MCU comes with 40Kb of RAM and 256Kb of flash memory.

## Hardware

![samr34-xpro image](https://www.microchip.com/content/dam/mchp/mrt-dam/devtools/3072-180605-wpd-dm320111-2.jpg)

An evaluation board for the radio certified WLR089 module exists as the WLR089 Xplained Pro, it is compatible with the `samr34-xpro`.

![wlr089-xpro](https://www.microchip.com/content/dam/mchp/mrt-dam/devtools/3411-264438-1598368489-c14679-200616-wsg-ev23m25a-wlr089-explained-pro-evaluation-board.jpg)


### MCU
| MCU        | SAMR34J18B            |
|:---------- |:--------------------- |
| Family     | ARM Cortex-M0+        |
| Vendor     | Atmel                 |
| RAM        | 32 kiB + 8 kiB battery RAM |
| Flash      | 256 KiB               |
| Frequency  | up to 48MHz           |
| FPU        | no                    |
| Timers     | 6 (3xTC (8,16,32 bit), 3xTCC (16 bit)) |
| ADCs       | 1x 12-bit (8 channels)|
| UARTs      | max 5 (shared with SPI and I2C) |
| SPIs       | max 5 (see UART)      |
| I2Cs       | max 5 (see UART)      |
| Vcc        | 1.8V - 3.6V           |
| Datasheet  | [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/SAMR34-R35-Low-Power-LoRa-Sub-GHz-SiP-Data-Sheet-DS70005356B.pdf) |
| Board Manual | [SAM R34 XPro](https://www.microchip.com/en-us/development-tool/DM320111)<br>[WLR089 XPro](https://www.microchip.com/en-us/development-tool/EV23M25A)|

 */
