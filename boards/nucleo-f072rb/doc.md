@defgroup   boards_nucleo-f072rb STM32 Nucleo-F072RB
@ingroup    boards_common_nucleo64
@brief      Support for the STM32 Nucleo-F072RB

## Overview

The Nucleo-F072RB is a board from ST's Nucleo family supporting a ARM Cortex-M0
STM32F072RB microcontroller with 16KiB of RAM and 128KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![Nucleo64 F072RB](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-f072rb.svg "Pinout for the Nucleo-F072RB (from user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 29)" width=50%

### MCU
| MCU        | STM32F072RB       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M0     |
| Vendor     | ST Microelectronics |
| RAM        | 16KiB             |
| Flash      | 128KiB            |
| Frequency  | up to 48MHz       |
| FPU        | no                |
| Timers     | 12 (2x watchdog, 1 SysTick, 8x 16-bit, 1x 32-bit) |
| ADCs       | 1x 12-bit (up to 16 channels) |
| UARTs      | 4                 |
| SPIs       | 2                 |
| I2Cs       | 2                 |
| CAN        | 1                 |
| USB        | 1                 |
| RTC        | 1                 |
| Vcc        | 2.0V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f072rb.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/resource/en/reference_manual/dm00031936.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/dm00051352.pdf) |
| Board Manual   | [Board Manual](http://www.st.com/resource/en/user_manual/dm00105823.pdf)|


## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F072RB   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (4 pins available)    |  |
|        | UART      | 3 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX), USART1 on PA10(RX)/PA09(TX) and USART3 on PC11(RX)/PC10(TX) |
|        | PWM       | 7 pins        | |
|        | ADC       | 6 pins        | |
|        | I2C       | no        | |
|        | SPI       | yes (SPI1)        | |
|        | USB       | no        | |
|        | Timer     | 1 32 bit timers (TIM1)        | |


## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F072RB is `nucleo-f072rb`.
