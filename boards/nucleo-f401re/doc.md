@defgroup    boards_nucleo-f401re STM32 Nucleo-F401RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F401RE

## Overview

The Nucleo-F401RE is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F401RE microcontroller with 96KiB of RAM and 512KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![Nucleo64 F401RE](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-f401re.svg "Pinout for the Nucleo-F401RE (from STM user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 32)" width=50%

### MCU

| MCU        | STM32F401RE       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4F    |
| Vendor     | ST Microelectronics |
| RAM        | 96KiB             |
| Flash      | 512KiB            |
| Frequency  | up to 84MHz       |
| FPU        | yes               |
| Timers     | 11 (2x watchdog, 1 SysTick, 6x 16-bit, 2x 32-bit [TIM2]) |
| ADCs       | 1x 12-bit (16 channels) |
| UARTs      | 3                 |
| SPIs       | 3                 |
| I2Cs       | 3                 |
| RTC        | 1                 |
| USB        | 1                 |
| Vcc        | 1.7V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f401re.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00096844.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/resource/en/programming_manual/dm00046982.pdf) |
| Board Manual | [Board Manual](http://www.st.com/resource/en/user_manual/dm00105823.pdf) |

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F401RE   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (7 pins available))   |  |
|        | UART      | 2 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX) and USART6 on PA12(RX)/PA11(TX) |
|        | I2C       | 1 I2C     | |
|        | SPI       | 1 SPI     | |
|        | USB       | no        | |
|        | Timer     | 2 32 bit timers       | |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F401RE is `nucleo-f401re`.

## Issues with old Hardware Revisions

Some Nucleo 64 Boards come in different hardware revisions, which is located
on a sticker at the backside of the board. As discussed in issue
[#6721](https://github.com/RIOT-OS/RIOT/issues/6721), there are some issues with
the external clock sources in revisions smaller than `C-03`. (This would also be
true for all boards with board ID `MB1136`).
The revision `C-01` has a different configuration for the high speed external
clock (HSE), which is used in RIOT. This could be fixed by reconfiguring some
solder bridges as mentioned in the [user
manual](http://www.st.com/resource/en/user_manual/dm00105823.pdf), section
6.7.1.
The other issue is, that revision `C-01` also has no 32kHz oscillator, so the
RTC module of RIOT cannot be used.
