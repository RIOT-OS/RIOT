@defgroup    boards_nucleo-l476rg STM32 Nucleo-L476RG
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-L476RG

## Overview

The Nucleo-L476RG is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32L476RG microcontroller with 128KiB of RAM and 1MiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Pinout

@image html pinouts/nucleo-l476rg.svg "Pinout for the Nucleo-L476RG (from STM user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 35)" width=50%

### MCU

| MCU        |   STM32L476RG     |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4F    |
| Vendor     | ST Microelectronics|
| RAM        | 128KiB            |
| Flash      | 1MiB              |
| Frequency  | up to 80MHz       |
| FPU        | yes               |
| Timers     | 16 (2x watchdog, 1 SysTick, 6x 16-bit, 2x 32-bit [TIM2])  |
| ADCs       | 3x 12-bit (up to 16 channels) |
| UARTs      | 6 (three USARTs, two UARTs and one Low-Power UART) |
| SPIs       | 3                 |
| I2Cs       | 3                 |
| RTC        | 1                 |
| CAN        | 1                 |
| Vcc        | 1.71 V - 3.6V     |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l476je.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/content/ccc/resource/technical/document/reference_manual/02/35/09/0c/4f/f7/40/03/DM00083560.pdf/files/DM00083560.pdf/jcr:content/translations/en.DM00083560.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf) |
| Board Manual   | [Board Manual](https://www.st.com/content/ccc/resource/technical/document/user_manual/98/2e/fa/4b/e0/82/43/b7/DM00105823.pdf/files/DM00105823.pdf/jcr:content/translations/en.DM00105823.pdf) |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-L476RG is `nucleo-l476rg`.
