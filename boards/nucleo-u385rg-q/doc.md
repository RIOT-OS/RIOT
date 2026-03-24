@defgroup    boards_nucleo-u385rg-q STM32 Nucleo-U385RG-Q
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-U385RG-Q

## Overview

The NUCLEO-U385RG-Q is a board from ST's Nucleo-64 family supporting the
STM32U385RG ultra-low-power microcontroller based on an ARM Cortex-M33 core
with TrustZone, 640KiB of RAM, and 1MiB of Flash.

You can find general information about the Nucleo-64 boards on the
@ref boards_common_nucleo64 page.

This RIOT port currently provides initial support for the board.
Basic bring-up and LED blinking have been verified. Other peripherals may
still be incomplete or untested.

## Hardware

### MCU

| MCU               | STM32U385RG |
|:------------------|:------------|
| Family            | ARM Cortex-M33 |
| Vendor            | STMicroelectronics |
| RAM               | 640KiB |
| Flash             | 1MiB |
| Frequency         | up to 96MHz |
| FPU               | yes |
| TrustZone         | yes |
| Datasheet         | [Datasheet](https://www.st.com/resource/en/datasheet/stm32u375ce.pdf) |
| Reference Manual  | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0487-stm32u3-series-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual| [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0264-stm32-cortexm33-mcus-and-mpus-programming-manual-stmicroelectronics.pdf) |
| Board Manual      | [Board Manual](https://www.st.com/resource/en/user_manual/um3062-stm32u3u5-nucleo64-boards-mb1841-stmicroelectronics.pdf) |

## Flashing the Board

A detailed description of the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).

The board name for the NUCLEO-U385RG-Q is `nucleo-u385rg-q`.

## Status

The following has been verified:

- Basic bring-up
- GPIO
- LED blinking

