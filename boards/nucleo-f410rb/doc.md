@defgroup    boards_nucleo-f410rb STM32 Nucleo-F410RB
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F410RB

## Overview

The Nucleo-F410RB is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F410RB microcontroller with 32KiB of RAM and 128KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![Nucleo64 F410RB](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-f410rb.svg "Pinout for the Nucleo-F410RB (from STM user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 36)" width=50%

### MCU

| MCU        | STM32F410RBT      |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4F    |
| Vendor     | ST Microelectronics |
| RAM        | 32KiB             |
| Flash      | 128KiB            |
| Frequency  | up to 100MHz      |
| FPU        | yes               |
| Timers     | 9 (2x watchdog, 1x SysTick, 4x 16-bit, 1x 32-bit [TIM2] and 1 low power timer) |
| ADCs       | 1x 12-bit (16 channels) |
| UARTs      | 3                 |
| SPIs       | 3                 |
| I2Cs       | 3                 |
| RTC        | 1                 |
| Vcc        | 1.8V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f410rb.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/resource/en/reference_manual/dm00180366.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/resource/en/programming_manual/dm00046982.pdf) |
| Board Manual | [Board Manual](http://www.st.com/resource/en/user_manual/dm00105823.pdf)|

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F410RB   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (no pin configured)   |  |
|        | UART      | 2 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX) and USART1 on PA10(RX)/PA9(TX) |
|        | I2C       | 1 I2C     | |
|        | SPI       | 1 SPI     | |
|        | USB       | no        | |
|        | Timer     | 2 32 bit timers       | |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F401RB is `nucleo-f401rb`.
