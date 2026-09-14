<!--
SPDX-FileCopyrightText: 2026 Hudson C. Dalpra
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup    boards_nucleo-g031k8 STM32 Nucleo-G031K8
@ingroup     boards_common_nucleo32
@brief       Support for the STM32 Nucleo-G031K8 board.

## General information

The Nucleo-G031K8 is a board from ST's Nucleo family supporting ARM Cortex-M0+
STM32G031K8 microcontroller with 8KiB of RAM and 64KiB of Flash.

You can find general information about the Nucleo32 boards on the
@ref boards_common_nucleo32 page.

## Pinout

<img src="nucleo-g031k8.svg" alt="Pinout for the Nucleo-G031K8 (from ST User Manual, UM2591, https://www.st.com/resource/en/user_manual/um2591-stm32g0-nucleo32-board-mb1455-stmicroelectronics.pdf, page 16)" width=25% />

## MCU

| MCU        |    STM32G031K8      |
|:---------- |:------------------- |
| Family     | ARM Cortex-M0+      |
| Vendor     | ST Microelectronics |
| RAM        | 8KiB                |
| Flash      | 64KiB               |
| Frequency  | up to 64MHz         |
| FPU        | no                  |
| Timers     | 11 (7x 16-bit, 1x 32-bit, 2x Watchdog, 1x Systick) |
| ADC        | 1x 12-bit (16 channels) |
| UARTs      | 3 (two USART)       |
| SPIs       | 2                   |
| CANs       | 0                   |
| RTC        | 1                   |
| I2Cs       | 2                   |
| Vcc        | 1.7V - 3.6V         |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32g031k8.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0444-stm32g0x1-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2591-stm32g0-nucleo32-board-mb1455-stmicroelectronics.pdf) |

## Flashing the board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-G031K8 is `nucleo-g031k8`.

## Reset configuration

Some boards ship with the `NRST_MODE` option byte set to `2` (the documented
default is `3`), i.e. in **User button mode** below. In this case, hardware
reset via NRST is unavailable, but RIOT works around this by resetting over SWD
(`reset_config none`), so `make flash` always works.

- **User button mode** (`NRST_MODE = 2`): PF2 is available as button BTN0 for
applications, but hardware reset via NRST is disabled. See warning below.

- **Reset mode** (`NRST_MODE = 1 or 3`): PF2 remains a hardware reset button,
but it cannot be used as an application user button.

@warning If your program crashes after startup, it might become impossible to
         reset and flash the microcontroller if the software reset
         is enabled! The microcontroller would be bricked in this case.<br/>
         You can try to power up the microcontroller with the BOOT0 pin pulled
         high to enter the bootloader and then connect with e.g.
         STM32CubeProgrammer to erase the flash memory. This suggestion
         is not tested yet!

To restore the hardware reset, set `NRST_MODE` to `1` or `3`. The easiest way
to do this is to use STM32CubeProgrammer, which is available for Linux,
Mac (x86 and ARM) and Windows on the
[ST website](https://www.st.com/en/development-tools/stm32cubeprog.html).

@note STM32CubeProgrammer v2.22 from February 2026 has a bug that prevents
writing the Option Bytes. Use an older or newer version instead!
