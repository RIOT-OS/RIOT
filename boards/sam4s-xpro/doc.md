@defgroup    boards_sam4s-xpro ATSAM4S Xplained Pro board
@ingroup     boards
@brief       Support for the ATSAM4S Xplained Pro board

## Overview

The `ATSAM4S Xplained Pro` is an evaluation board by Microchip (formerly Atmel)
featuring a SAM4SD32 (Cortex-M4) SoC running up to 120 MHz. This MCU comes with
160Kb of RAM and 2048Kb of flash memory.

## Hardware

![sam4s-xpro image](https://www.microchip.com/content/dam/mchp/mrt-dam/devtools/1801-atsam4s-xpro.jpg)


### MCU
| MCU        | ATSAM4SD32C      |
|:------------- |:--------------------- |
| Family | ARM Cortex-M4    |
| Vendor | Microchip |
| RAM        | 160Kb |
| Flash      | 2048Kb             |
| Frequency  | up to 120MHz |
| FPU        | no                |
| Timers | 2 three channels (16-bit)    |
| ADCs       | 1x 12-bit (16 channels)           |
| UARTs      | 2 UARTs (+ 2 USARTs shared with SPIs) |
| SPIs       | 2 shared with USARTs                  |
| I2Cs       | 2    |
| Datasheet  | [Datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/Atmel-11100-32-bitCortex-M4-Microcontroller-SAM4S_Datasheet.pdf) |
| Board Manual   | [Board Manual](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/UserGuides/Atmel-42075-SAM4S-Xplained-Pro_User-Guide.pdf)|

### User Interface

1 User button and 1 LED:

| Device | PIN |
|:------ |:--- |
| LED0   | PC23 |
| SW0 (button) | PA02 |

### Flashing options
By default, openOCD is used for both flashing and debugging.
EDBG programmer is also available with:
`PROGRAMMER=edbg BOARD=sam4s-xpro make -C tests/leds flash`
