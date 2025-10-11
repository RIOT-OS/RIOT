@defgroup    boards_nucleo-l011k4 STM32 Nucleo-L011K4
@ingroup     boards_common_nucleo32
@brief       Support for the STM32 Nucleo-L011K4

## Overview

The Nucleo-L011K4 is a board from ST's Nucleo family supporting ARM-Cortex-M0+
STM32L011K4 microcontroller with 2KiB of RAM and 16KiB of Flash.

## Pinout

@image html pinouts/nucleo-l432kc-and-more.svg "Pinout for the Nucleo-L011K4 (from STM user manual, UM1956, https://www.st.com/resource/en/user_manual/um1956-stm32-nucleo32-boards-mb1180-stmicroelectronics.pdf, page 32)" width=25%

### MCU

| MCU        |    STM32L011K4      |
|:---------- |:------------------- |
| Family     | ARM Cortex-M0+      |
| Vendor     | ST Microelectronics |
| RAM        | 2KiB                |
| Flash      | 16KiB               |
| Frequency  | up to 32MHz         |
| FPU        | no                  |
| Timers     | 7 (3x 16-bit, 1x RTC, 1x Systick, 2x Watchdog) |
| ADC        | 1x 12-bit (10 channels) |
| UARTs      | 2 (one USART and one Low-Power UART) |
| SPIs       | 1                   |
| CANs       | 0                   |
| RTC        | 1                   |
| I2Cs       | 1                   |
| Vcc        | 1.65V - 3.6V        |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l011k4.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0377-ultralowpower-stm32l0x1-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um1956-stm32-nucleo32-boards-mb1180-stmicroelectronics.pdf) |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-L011K4 is `nucleo-l011k4`.
