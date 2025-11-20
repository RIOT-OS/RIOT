@defgroup    boards_nucleo-f446ze STM32 Nucleo-F446ZE
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F446ZE

## Overview

The Nucleo-F446ZE is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F446ZE microcontroller with 128KiB of RAM and 512KiB of ROM Flash.

You can find general information about the Nucleo144 boards on the
@ref boards_common_nucleo144 page.

## Hardware

![Nucleo144 F446ZE](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/nucleo144-board.png)

## Pinout

@image html pinouts/nucleo-f446ze-and-f722ze.svg "Pinout for the Nucleo-F446ZE (from STM user manual, UM1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 35)" width=50%

### MCU

| MCU        | STM32F446ZE       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4F    |
| Vendor     | ST Microelectronics |
| RAM        | 128KiB            |
| Flash      | 512KiB            |
| Frequency  | up to 180MHz      |
| FPU        | yes               |
| Timers     | 17 (2x watchdog, 1 SysTick, 12x 16-bit, 2x 32-bit [TIM2]) |
| ADCs       | 3x 12-bit (24 channels) |
| UARTs      | 6                 |
| SPIs       | 4                 |
| I2Cs       | 4                 |
| RTC        | 1                 |
| CAN        | 2                 |
| USB        | 1                 |
| Vcc        | 1.8V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f446ze.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00135183.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/dm00046982.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518.pdf)|

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F446ZE   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (9 pins available)    |  |
|        | UART      | 3 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX), USART3 on PC11(RX)/PC10(TX) and USART1 on PA10(RX)/PA9(TX) |
|        | I2C       | one I2C       | |
|        | SPI       | one SPI       | |
|        | USB       | no        | |
|        | Timer     | one 32 bit timer      | |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F446ZE is `nucleo-f446ze`.
