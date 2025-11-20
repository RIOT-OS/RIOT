@defgroup    boards_nucleo-c071rb STM32 Nucleo-C071RB
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-C071RB

## Overview

The Nucleo-C071RB is a board from ST's Nucleo family supporting a ARM
Cortex-M0+ STM32C071RB microcontroller with 24KiB of RAM and 128KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Pinout

See Board Manual referenced below, section 7 pages 19-21.

### MCU

| MCU        | STM32C071RB         |
|:---------- |:------------------- |
| Family     | ARM Cortex-M0+      |
| Vendor     | ST Microelectronics |
| RAM        | 24KiB               |
| Flash      | 128KiB              |
| Frequency  | up to 48MHz         |
| FPU        | no                  |
| Timers     | 12 (2x watchdog, 1 SysTick, 5x 16-bit) |
| ADCs       | 1x 12-bit (up to 19 channels) |
| UARTs      |  2                  |
| SPIs       |  1                  |
| I2Cs       |  1                  |
| RTC        |  1                  |
| Vcc        |  2.0V - 3.6V        |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32c071rb.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0490-stm32c0x1-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf) |
| Board Manual   | [Board Manual](https://www.st.com/resource/en/user_manual/um3353-stm32-nucleo64-board-mb2046-stmicroelectronics.pdf) |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-C071RB is `nucleo-c071rb`.
