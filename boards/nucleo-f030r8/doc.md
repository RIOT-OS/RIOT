@defgroup    boards_nucleo-f030r8 STM32 Nucleo-F030R8
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F030R8

## Overview

The Nucleo-F030R8 is a board from ST's Nucleo family supporting a ARM Cortex-M0
STM32F030R8 microcontroller with 8KiB of RAM and 64KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![Nucleo64 F030R8](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-f030r8.svg "Pinout for the Nucleo-F030R8 from STM user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 28)" width=50%

### MCU
| MCU        | STM32F030R8                                                          |
|:---------- |:-------------------------------------------------------------------- |
| Family     | ARM Cortex-M0                                                        |
| Vendor     | ST Microelectronics                                                  |
| RAM        | 8KiB                                                                 |
| Flash      | 64KiB                                                                |
| Frequency  | up to 48MHz                                                          |
| FPU        | no                                                                   |
| Timers     | 10 (2x watchdog, 1 SysTick, 7x 16-bit)                               |
| ADCs       | 1x 12-bit                                                            |
| UARTs      | 2                                                                    |
| SPIs       | 2                                                                    |
| I2Cs       | 2                                                                    |
| RTC        | 1                                                                    |
| Vcc        | 2.4V - 3.6V                                                          |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f030r8.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00091010.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/resource/en/programming_manual/dm00051352.pdf) |
| Board Manual   | [Board Manual](http://www.st.com/resource/en/user_manual/dm00105823.pdf)|



## Implementation Status
| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F030R8   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (4 pins available)  |  |
|        | UART      | 2 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX) and USART1 on PA10(RX)/PA09(TX) |
|        | ADC       | 6 pins        | |
|        | I2C       | no        | |
|        | SPI       | no        | |
|        | USB       | no        | |
|        | Timer     | 3 16 bit timers       | |


## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F030R8 is `nucleo-f030r8`.
