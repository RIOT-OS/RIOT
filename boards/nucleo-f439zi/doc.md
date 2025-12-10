@defgroup    boards_nucleo-f439zi STM32 Nucleo-F439ZI
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F439ZI

## Overview

The Nucleo-F439ZI is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32F439ZI microcontroller with, 256KiB of RAM and 2MiB of Flash.

You can find general information about the Nucleo144 boards on the
@ref boards_common_nucleo144 page.

## Pinout

@image html pinouts/nucleo-f207zg-and-more.svg "Pinout for the Nucleo-F439ZI (from STM user manual, UM1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 32)" width=50%

### MCU

| MCU          | STM32F439ZI
|:-------------|:--------------------|
| Family       | ARM Cortex-M4F      |
| Vendor       | ST Microelectronics |
| RAM          | 256KiB              |
| Flash        | 2MiB                |
| Frequency    | up to 180 MHz       |
| FPU          | yes                 |
| Ethernet     | 10/100 Mbps         |
| Timers       | 17 (2x watchdog, 1 SysTick, 2x 32bit, 12x 16bit) |
| ADCs         | 3x 12 bit (up to 24 channels) |
| UARTs        | 4                   |
| I2Cs         | 3                   |
| SPIs         | 6                   |
| CAN          | 2                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f439zi.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518-stm32-nucleo-144-boards-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F439ZI is `nucleo-f439zi`.
