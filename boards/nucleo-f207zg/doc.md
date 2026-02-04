@defgroup    boards_nucleo-f207zg STM32 Nucleo-F207ZG
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F207ZG

## Overview

The Nucleo-F207ZG is a board from ST's Nucleo family supporting a ARM
Cortex-M3 STM32F207ZG microcontroller with 128KiB of SRAM and 1MiB of ROM Flash.

You can find general information about the Nucleo144 boards on the
@ref boards_common_nucleo144 page.

## Hardware

![Nucleo144 F207ZG](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/nucleo144-board.png)

## Pinout

@image html pinouts/nucleo-f207zg-and-more.svg "Pinout for the Nucleo-F207ZG (from STM user manual, UM1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 32)" width=50%

### MCU

| MCU        | STM32F207ZG       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M3     |
| Vendor     | ST Microelectronics |
| RAM        | 128KiB            |
| Flash      | 1MiB              |
| Frequency  | up to 120MHz      |
| FPU        | no                |
| Timers     | 17 (2x watchdog, 1 SysTick, 12x 16-bit, 2x 32-bit [TIM2]) |
| ADCs       | 3x 12-bit (24 channels) |
| UARTs      | 6                 |
| SPIs       | 3                 |
| I2Cs       | 3                 |
| RTC        | 1                 |
| CAN        | 2                 |
| USB        | 1                 |
| Ethernet   | 1                 |
| Vcc        | 1.8V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f207zg.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/resource/en/reference_manual/cd00225773.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/resource/en/programming_manual/cd00228163.pdf) |
| Board Manual | [Board Manual](http://www.st.com/resource/en/user_manual/dm00244518.pdf) |

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F207ZG   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (7 pins available in see [PR #6192](https://github.com/RIOT-OS/RIOT/pull/6192))   |  |
|        | UART      | 2 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX) and USART6 on PA12(RX)/PA11(TX) |
|        | I2C       | 1 I2C     | |
|        | SPI       | 1 SPI     | |
|        | USB       | no        | |
|        | Timer     | 2 32 bit timers       | |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F207ZG is `nucleo-f207zg`.
