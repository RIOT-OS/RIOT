@defgroup    boards_rpi_pico_2 Raspberry Pi Pico 2
@ingroup     boards
@brief       Support for the RP2350 based Raspberry Pi Pico board

## Overview

The Raspberry Pi Pico 2 is a microcontroller board based on the RP2350 chip, featuring dual-core Arm Cortex-M0+ processors and RISC-V Hazard secondary architecture. It is designed for a wide range of applications, from hobbyist projects to professional embedded systems for a fairly affordable price.

![The Raspberry Pi Pico 2 Board](https://www.raspberrypi.com/documentation/microcontrollers/images/pico-2.png)

## Hardware

| MCU        | RP2350                                                      |
|:-----------|:------------------------------------------------------------|
| Family     | Dual Cortex-M33 or Hazard3 (RISC-V) @ 150 MHz               |
| Vendor     | Raspberry Pi                                                |
| RAM        | 520 kB on-chip SRAM (10 independent banks)                  |
| Flash      | Up to 16 MB external QSPI flash (expandable to 32 MB)       |
| Frequency  | up to 150 MHz                                               |
| Security   | Boot signing, key storage, SHA-256 accelerator              |
| PIOs       | 12 state machines                                           |
| UARTs      | 2                                                           |
| SPIs       | 2                                                           |
| I2Cs       | 2                                                           |
| PWM        | 24 channels                                                 |
| USB        | USB 1.1 controller with host and device support             |
| Power      | On-chip switched-mode power supply with LDO sleep mode      |
| OTP        | 8 kB of one-time-programmable storage                       |
| Datasheet  | [RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf) |

## User Interfaces

| Interface  | Description                                                  |
|:-----------|:-------------------------------------------------------------|
| LED0       | User LED (GPIO 0 at Pin 25)                                  |
| SW0        | Button used in flash process, can be accessed using registers but difficult |

## Pinout

![Pinout Diagram](https://www.raspberrypi.com/documentation/microcontrollers/images/pico-2-r4-pinout.svg)

## Flashing the Board

### Flashing using OpenOCD

The Raspberry Pi Pico 2 has a built-in bootloader that allows flashing via USB. However, you can also use OpenOCD for flashing the board.

If you have two Raspberry Pi Pico boards, you can utilize one as a programmer to program the other board.
Please refer to the [Debugprobe documentation](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html#getting-started) for more details.

To do this, you need to connect the board to your computer and use the following command:

```bash
PROGRAMMER=openocd make flash
```

You can then debug your application using GDB with the following command:

```bash
PROGRAMMER=openocd make debug
```

### Flashing using Picotool

Simply connect the board to your computer via USB and use the following command:

```bash
BOARD=rpi-pico-2 make flash
```

This is the default method for flashing the Raspberry Pi Pico 2. However, it does not allow for debugging using GDB.

