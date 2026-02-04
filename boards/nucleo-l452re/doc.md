@defgroup    boards_nucleo-l452re STM32 Nucleo-L452RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-L452RE

## Overview

The Nucleo-L452RE is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32L452RE microcontroller with 160KiB of RAM and 512KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Pinout

@image html pinouts/nucleo-l452re.svg "Pinout for the Nucleo-L452RE (from STM board manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 34)" width=50%

### MCU

| MCU        |   STM32L452RE       |
|:---------- |:------------------- |
| Family     | ARM Cortex-M4F      |
| Vendor     | ST Microelectronics |
| RAM        | 160KiB              |
| Flash      | 512KiB              |
| Frequency  | up to 80MHz         |
| FPU        | yes                 |
| Timers     | 12 (2x watchdog, 1 SysTick, 8x 16-bit, 1x 32-bit) |
| ADCs       | 1x 12-bit           |
| UARTs      | 5 (one UART, three USARTs and one Low-Power UART) |
| SPIs       | 3                   |
| I2Cs       | 4                   |
| RTC        | 1                   |
| CAN        | 1                   |
| Vcc        | 1.71 V - 3.6V       |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l452cc.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0394-stm32l41xxx42xxx43xxx44xxx45xxx46xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf) |
| Board Manual   | [Board Manual](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf) |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-L452RE is `nucleo-l452re`.
