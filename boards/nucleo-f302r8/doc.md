@defgroup    boards_nucleo-f302r8 STM32 Nucleo-F302R8
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F302R8

## Overview

The [Nucleo-F302R8](http://www.st.com/en/evaluation-tools/nucleo-f302r8.html)
is a board from ST's Nucleo family supporting a ARM Cortex-M4
[STM32F302R8](http://www.st.com/en/microcontrollers/stm32f302r8.html)
microcontroller with 16KiB of RAM and 64KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![nucleo image](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-f302r8.svg "Pinout for the Nucleo-F302R8 (from user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 30)" width=50%

### MCU
| MCU        | STM32F302R8           |
|:---------- |:--------------------- |
| Family     | ARM Cortex-M4F        |
| Vendor     | ST Microelectronics   |
| RAM        | 16KiB                 |
| Flash      | 64KiB                 |
| Frequency  | up to 72MHz           |
| FPU        | yes                   |
| Timers     | 9 (5x 16-bit, 1x 32-bit [TIM2], 2x watchdog, 1x systick) |
| ADCs       | 1 with selectable resolution (6,8,10,12-bit) |
| UARTs      | 3                     |
| SPIs       | 2                     |
| I2Cs       | 3                     |
| RTC        | 1                     |
| CAN        | 1                     |
| USB        | 1                     |
| Vcc        | 2.0V - 3.6V           |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f302r8.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/resource/en/reference_manual/dm00094349.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/resource/en/programming_manual/dm00046982.pdf) |
| Board Manual | [Board Manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00105823.pdf) |


## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F302R8   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | one PWM (D5)  | |
|        | UART      | three UART    | |
|        | I2C       | no | |
|        | SPI       | one SPI (SPI2)    | |
|        | USB       | no        | |
|        | Timer     | one 32 timer (TIM2)   | |


## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F302R8 is `nucleo-f302r8`.
