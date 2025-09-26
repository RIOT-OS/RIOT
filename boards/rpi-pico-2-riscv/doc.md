@defgroup    boards_rpi_pico_2 Raspberry Pi Pico 2
@ingroup     boards
@brief       Support for the RP2350 based Raspberry Pi Pico board

@warning The support for the Raspberry Pi Pico 2 is still in a very early stage!
See [Known Issues](#rpi_pico_2_known_issues).

## Overview

The Raspberry Pi Pico 2 is a microcontroller board based on the RP2350 chip,
featuring dual-core Arm Cortex-M0+ processors and RISC-V Hazard secondary
architecture. It is designed for a wide range of applications,
from hobbyist projects to professional embedded systems
for a fairly affordable price.

![The Raspberry Pi Pico 2 Board](https://www.raspberrypi.com/documentation/microcontrollers/images/pico-2.png)

## Hardware

| MCU        | RP2350                                                      |
|:-----------|:------------------------------------------------------------|
| Family     | Dual Cortex-M33 or Hazard3 (RISC-V)                         |
| Vendor     | Raspberry Pi                                                |
| RAM        | 520 kB on-chip SRAM (10 independent banks)                  |
| Flash      | Up to 16 MB external QSPI flash (Pico 2 has 4 MB by default)|
| Frequency  | up to 150 MHz (Set to 125 MHz in RIOT)                      |
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

The Raspberry Pi Pico 2 has a built-in bootloader that allows flashing via USB.
However, you can also use OpenOCD for flashing the board.
If you are using picotool, you need to hold the bootselect button
(the only button on the board) while connecting the board to
your computer via USB. This will put the board into bootloader mode,
allowing you to flash it.

### Flashing using OpenOCD

If you have two Raspberry Pi Pico boards,
you can utilize one as a programmer to program the other board.

Please refer to the
[Debugprobe documentation](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html#getting-started)
for more details.

Note that Raspberry Pi actually uses their own OpenOCD fork, which is available
in the [RP2040 OpenOCD repository](https://github.com/raspberrypi/openocd).
While technically you can use the standard OpenOCD,
it is recommended to use the Raspberry Pi fork for better compatibility with the
RP2350, as its still fairly "new" and under development,
which is why even their own Pico SDK Extension
uses the Raspberry Pi fork of OpenOCD, instead of the standard one.

To do this, you need to connect the board to your computer
and use the following command:

```bash
PROGRAMMER=openocd BOARD=rpi-pico-2 make flash
```

You can then debug your application using GDB with the following command:

```bash
PROGRAMMER=openocd BOARD=rpi-pico-2 make debug
```

### Flashing using Picotool

Simply connect the board to your computer via USB and use the following command:

```bash
BOARD=rpi-pico-2 make flash
```

This is the default method for flashing the Raspberry Pi Pico 2.
However, it does not allow for debugging using GDB.

@note When programming the board with the Picotool for the first time,
RIOT will download and install the Picotool locally in the RIOT folder.
This process will take some minutes to complete.

## Known Issues {#rpi_pico_2_known_issues}

Currently RP2350 support is rather minimal,
as such peripheral support is extremely limited.
The following peripherals are supported:

- GPIO
- Non-configurable write-only UART (UART0 using Pin 0 and 1)
    - The UART Baudrate is set to 115200.
    - UART does not work via USB, you need to connect it directly to the GPIO pins.

More peripherals will be added in the future.
It should also be noted that we currently only support the Cortex M33 cores,
not the RISC-V Hazard cores.
