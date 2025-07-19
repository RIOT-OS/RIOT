@defgroup    boards_arduino-due Arduino Due
@ingroup     boards
@brief       Support for the Arduino Due board

\section arduino_due Arduino-Due

## Table of Contents {#arduino_due_toc}

1. [Overview](#arduino_due_overview)
2. [Hardware](#arduino_due_hardware)
    1. [MCU](#atmel_sam3x8e_mcu)
    2. [Board Configuration](#arduino_due_board_configuration)
    3. [Board Pinout](#arduino_due_pinout)
3. [Flashing the Device](#arduino_due_flashing)

## Overview {#arduino_due_overview}

The Arduino Due is a microcontroller board based on the Atmel SAM3X8E ARM Cortex-M3 CPU.
It is the first Arduino board based on a 32-bit ARM core microcontroller.

\image html "https://store.arduino.cc/cdn/shop/files/A000062_00.front_1000x750.jpg" "Arduino Due board" width=750px

## Hardware {#arduino_due_hardware}

### MCU {#atmel_sam3x8e_mcu}

The board features a SAM3X8E MCU (ARM Cortex-M3) running at up to 84MHz.
It embeds 96 KiB of SRAM and 512 KiB of flash memory.
The board reference manual is available [here](https://docs.arduino.cc/resources/datasheets/A000062-datasheet.pdf)
and the MCU datasheet is available [here](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-11057-32-bit-Cortex-M3-Microcontroller-SAM3X-SAM3A_Datasheet.pdf).

### Board configuration {#arduino_due_board_configuration}
The Arduino Due provides the following features:
- 54 I/Os (of which 12 PWM outputs)
- 12 analog inputs
- 4 UARTs
- 1 USB OTG
- 2 DAC outputs
- 2 TWI
- 1 power jack
- 1 SPI header
- 1 JTAG header
- 2 CAN Buses
- A reset button
- An erase button

### Board pinout {#arduino_due_pinout}

\image html "https://content.arduino.cc/assets/Pinout-Due_latest.png" "Arduino Due pinout"

The board schematics are available [here](https://docs.arduino.cc/resources/schematics/A000056-schematics.pdf).

## Flashing the device {#arduino_due_flashing}
Flashing RIOT on the Arduino Due is quite straight forward as it relies on
the preinstalled arduino bootloader. Connect your board using the programming
port to your host computer and type:

`make BOARD=arduino-due flash`
