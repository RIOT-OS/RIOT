@defgroup    boards_nucleo-f722ze STM32 Nucleo-F722ZE
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F722ZE

## Overview

The Nucleo-F722ZE is a board from ST's Nucleo family supporting ARM Cortex-M7
STM32F722ZE microcontroller with 256KiB of RAM and 512KiB of Flash.

You can find general information about the Nucleo144 boards on the
@ref boards_common_nucleo144 page.

## Pinout

@image html pinouts/nucleo-f446ze-and-f722ze.svg "Pinout for the Nucleo-F722ZE (from STM user manual, UM1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 35)" width=50%

### MCU

| MCU          | STM32F722ZE
|:-------------|:--------------------|
| Family       | ARM Cortex-M7       |
| Vendor       | ST Microelectronics |
| RAM          | 256KiB              |
| Flash        | 512KiB              |
| Frequency    | up to 216 MHz       |
| FPU          | yes                 |
| Timers       | 18 (2x watchdog, 1 SysTick, 2x 32-bit, 13x 16-bit) |
| ADCs         | 3x 12 bit (up to 24 channels) |
| UARTs        | 4                   |
| I2Cs         | 3                   |
| SPIs         | 5                   |
| CAN          | 1                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f722ic.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0431-stm32f72xxx-and-stm32f73xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0253-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518-stm32-nucleo-144-boards-stmicroelectronics.pdf)|


## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F722ZE is `nucleo-f722ze`.
