@defgroup    boards_nucleo-l031k6 STM32 Nucleo-L031K6
@ingroup     boards_common_nucleo32
@brief       Support for the STM32 Nucleo-L031K6

## Overview

The Nucleo-L031K6 is a board from ST's Nucleo family supporting ARM Cortex-M0
STM32L031K6T6 microcontroller with 8KiB of RAM and 32KiB of Flash.

You can find general information about the Nucleo32 boards on the
@ref boards_common_nucleo32 page.

## Pinout

@image html pinouts/nucleo-l432kc-and-more.svg "Pinout for the Nucleo-L031K6 (from STM user manual, UM1956, https://www.st.com/resource/en/user_manual/um1956-stm32-nucleo32-boards-mb1180-stmicroelectronics.pdf, page 32)" width=25%

### MCU

| MCU        |    STM32L031K6      |
|:---------- |:------------------- |
| Family     | ARM Cortex-M0+      |
| Vendor     | ST Microelectronics |
| RAM        | 8KiB                |
| Flash      | 32KiB               |
| Frequency  | up to 32MHz         |
| FPU        | no                  |
| Timers     | 8 (4x 16-bit, 1x RTC, 1x Systick, 2x Watchdog) |
| ADC        | 1x 12-bit (10 channels) |
| UARTs      | 2 (one USART and one Low-Power UART) |
| SPIs       | 2                   |
| CANs       | 0                   |
| RTC        | 1                   |
| I2Cs       | 1                   |
| Vcc        | 1.65V - 3.6V        |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l031k6.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0377-ultralowpower-stm32l0x1-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um1956-stm32-nucleo32-boards-mb1180-stmicroelectronics.pdf) |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-L031K6 is `nucleo-l031k6`.
