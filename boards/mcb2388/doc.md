@defgroup    boards_mcb2388 MCB2388 Evaluation Board
@ingroup     boards
@brief       Support for the MCB2388 Evaluation Board

## Overview

![MCB2388 image](http://www.keil.com/support/man/docs/mcb2300/mcb2388_board.jpg)

## Hardware

| MCU:         | LPC2388            |
|--------------|--------------------|
| Family       | ARM7TDMI-S         |
| Vendor       | NXP                |
| RAM          | 64KiB              |
|              | 16KiB USB RAM      |
|              | 16KiB Ethernet RAM |
|              |  2KiB Backup RAM   |
| Flash        | 512KiB             |
| Frequency    | up to 72 MHz       |
| FPU          | no                 |
| Timers       | 4 (32-bit)         |
| ADCs         | 1x 10-bit (8 channels) |
| DACs         | 1x 10-bit (1 channel)  |
| UARTs        | 4                  |
| SPIs         | 2                  |
| I2Cs         | 3                  |
| Vcc          | 3.0V - 3.63V       |
| Datasheet    | [Datasheet](https://www.nxp.com/docs/en/user-guide/UM10211.pdf) |
| Board Manual | [Board Manual](https://www.keil.com/mcb2300/mcb2388.asp)        |

### User Interface

1 User button, 8 LED, 1 potentiometer, 1 speaker:

| Device | PIN   |
|:------ |:----- |
| LED0   | P2.0  |
| LED1   | P2.1  |
| LED2   | P2.2  |
| LED3   | P2.3  |
| LED4   | P2.4  |
| LED5   | P2.5  |
| LED6   | P2.6  |
| LED7   | P2.7  |
| SW0    | P2.10 |
| POT0   | P0.23 |
| SPK0   | P0.26 |

## Implementation Status

| Peripheral | Supported | Comments                      |
|:---------- |:--------- |:----------------------------- |
| ADC        | yes       |                               |
| CAN        | no        |                               |
| DMA        | partly    | only for MCI                  |
| Ethernet   | no        |                               |
| GPIO       | yes       |                               |
| I2C        | yes       |                               |
| I2S        | no        |                               |
| MCI        | yes       | Media Card (SD/MMC) Interface |
| PWM        | yes       |                               |
| RTC        | yes       |                               |
| SPI        | yes       |                               |
| Timer      | yes       |                               |
| UART       | yes       |                               |
| USB        | no        |                               |
| Watchdog   | no        |                               |

# More info

## Board Schematics
[Board schematics](http://www.keil.com/mcb2300/mcb2300-schematics.pdf)

## Flashing

The MCB2388 is programmed via an ISP bootloader over UART. For this you have to
connect an USB-RS232 converter to the COM0 port.

Make sure to have the ISP Jumper set.

Connect the MCB2388 to your PC using the serial port and run from within the
folder of your application to flash:

    BOARD=mcb2388 make flash

You'll notice that during the first invocation takes a bit longer, as the tool
`lpc2k_pgm` used to flash the MCB2388 is also compiled.

You can connect to the shell by running:

    BOARD=mcb2388 make term

## Serial Terminal

By default the RST line is connected to the RTS/CTS line of COM0.
This will pull the MCU into reset when connecting a terminal. Use `pyterm -tg` to
toggle the pins and complete the reset.

If you want to connect a terminal application that does not support this, remove
the RST jumper from the board.
You will not be able to flash the board using `lpc2k_pgm` if the RST jumper is
disconnected.

## Debugging

The development kit comes with a ULINK-ME Debug Unit that supports the CMSIS-DAP
protocol.
Connect it with the JTAG port on the board, the debugging should work by typing

    BOARD=mcb2388 make debug

### Flashing via OpenOCD

Theoretically you should be able to flash also via JTAG.
Unfortunately OpenOCD support for this chip family seems to be pretty buggy.
I was unable to flash the board using the ULINK-ME adapter and OpenOCD. (2019)

The board might get stuck when using OpenOCD, you have to power-cycle it to
being able to use ISP programming again.
