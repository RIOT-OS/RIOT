@defgroup    boards_nucleo-f411re STM32 Nucleo-F411RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F411RE

## Overview

The Nucleo-F411RE is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F411RE microcontroller with 128KiB of RAM and 512KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![Nucleo64 F411RE](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-f411re.svg "Pinout for the Nucleo-F411RE (from STM user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 32)" width=50%

### MCU

| MCU        | STM32F411RE       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4F    |
| Vendor     | ST Microelectronics |
| RAM        | 128KiB            |
| Flash      | 512KiB            |
| Frequency  | up to 100MHz      |
| FPU        | yes               |
| Timers     | 11 (2x watchdog, 1 SysTick, 7x 16-bit, 1x 32-bit [TIM2]) |
| ADCs       | 1x 12-bit (16 channels) |
| UARTs      | 3                 |
| SPIs       | 5                 |
| I2Cs       | 3                 |
| RTC        | 1                 |
| USB        | 1                 |
| Vcc        | 1.7V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f411re.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/resource/en/reference_manual/dm00119316.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/resource/en/programming_manual/dm00046982.pdf) |
| Board Manual | [Board Manual](http://www.st.com/resource/en/user_manual/dm00105823.pdf) |

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F411RE   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (7 pins available))   |  |
|        | UART      | 3 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX), USART1 on PA10(RX)/PA9(TX) and USART6 on PA12(RX)/PA11(TX) |
|        | I2C       | 1 I2C     | |
|        | SPI       | 1 SPI     | |
|        | USB       | no        | |
|        | Timer     | 1 32 bit timer (TIM5)     | |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F411RE is `nucleo-f411re`.
