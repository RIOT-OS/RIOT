@defgroup    boards_nucleo-wl55jc STM32 Nucleo-WL55JC
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-W55JC

## Overview

The Nucleo-W55JC is a board from ST's Nucleo family supporting ARM dual-core
Cortex-M4/M0 STM ultra-low-power microcontroller with ultra-low-power LPWAN
radio solution, 64KiB of RAM and 256KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

### MCU


| MCU        |   STM32WL55JC                                             |
|:---------- |:--------------------------------------------------------- |
| Family     | ARM Cortex-M4 / ARM Cortex-M0+ Dual core                  |
| Vendor     | ST Microelectronics                                       |
| RAM        | 64KiB                                                     |
| Flash      | 256KiB                                                    |
| Frequency  | up to 48MHz                                               |
| FPU        | no                                                        |
| Timers     | 10 (2x watchdog, 1 SysTick, 6x 16-bit, 1x 32-bit [TIM2])  |
| ADCs       | 1x 12-bit (12 channel )                                   |
| UARTs      | 3 (two UARTs and one Low-Power UART)                      |
| SPIs       | 2                                                         |
| I2Cs       | 3                                                         |
| RTC        | 1                                                         |
| Vcc        | 1.8 V - 3.6V                                              |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32wl55jc.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00451556-stm32wl5x-advanced-armbased-32bit-mcus-with-subghz-radio-solution-stmicroelectronics.pdf) |
| Programming Manual 1 | [Reference Manual - M4 ](https://www.st.com/resource/en/programming_manual/dm00046982-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf) |
| Programming Manual 2 | [Reference Manual - M0](https://www.st.com/resource/en/programming_manual/dm00104451-cortexm0-programming-manual-for-stm32l0-stm32g0-stm32wl-and-stm32wb-series-stmicroelectronics.pdf) |
| Board Manual   | [Board Manual](https://www.st.com/resource/en/data_brief/nucleo-wl55jc.pdf) |
| Board Schematic | [Board Schematic](https://www.st.com/resource/en/user_manual/dm00622917-stm32wl-nucleo64-board-mb1389-stmicroelectronics.pdf) |
| Additional Reference | [Developing LoRaWAN Application](https://www.st.com/resource/en/application_note/dm00660451-how-to-build-a-lora-application-with-stm32cubewl-stmicroelectronics.pdf)

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-WL55JC is `nucleo-wl55jc`.
