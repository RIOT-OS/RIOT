@defgroup    boards_nucleo-f303re STM32 Nucleo-F303RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F303RE

## Overview

The Nucleo-F303RE is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F303RE microcontroller with 64KiB of RAM and 512KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![Nucleo64 F303RE](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)
![nucleo-f303 pinout](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/nucleo-f303_pinout.png)

## Pinout

@image html pinouts/nucleo-f303re.svg "Pinout for the Nucleo-F303RE (from user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 31)" width=50%

### MCU
| MCU        | STM32F303RE       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4F    |
| Vendor     | ST Microelectronics |
| RAM        | 64KiB             |
| Flash      | 512KiB            |
| Frequency  | up to 72MHz       |
| FPU        | yes               |
| Timers     | 13 (9x 16-bit, 1x 32-bit [TIM2], 1x Systick, 2x watchdog) |
| ADCs       | 4x 12-bit (22 channels) |
| UARTs      | 5                 |
| SPIs       | 4                 |
| I2Cs       | 3                 |
| RTC        | 1                 |
| CAN        | 1                 |
| USB        | 1                 |
| Vcc        | 2.0V - 3.6V       |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f303re.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/web/en/resource/technical/document/reference_manual/DM00043574.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/web/en/resource/technical/document/programming_manual/DM00046982.pdf) |
| Board Manual | [Board Manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00105823.pdf) |



## Implementation Status
| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F303RE   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | one PWM   | |
|        | UART      | three UART    | |
|        | I2C       | two I2C   | |
|        | SPI       | two SPI   | |
|        | USB       | no        | |
|        | Timer     | one 32 timer  | |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F303RE is `nucleo-f303re`.
