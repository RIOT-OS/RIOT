@defgroup    boards_nucleo-f091rc STM32 Nucleo-F091RC
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F091RC

## Overview

The Nucleo-F091RC is a board from ST's Nucleo family supporting a ARM Cortex-M0
STM32F091RC microcontroller with 32KiB of RAM and 256KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![Nucleo64 F091RC](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-f091rc.svg "Pinout for the Nucleo-F091RC (from user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 29)" width=50%

### MCU
| MCU        | STM32F091RC       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M0     |
| Vendor     | ST Microelectronics |
| RAM        | 32KiB             |
| Flash      | 256KiB            |
| Frequency  | up to 48MHz       |
| FPU        | no                |
| Timers     | 12 (8x 16-bit, 1x 32-bit [TIM2], 1x Systick, 2x watchdog) |
| ADCs       | 1x 12-bit (up to 16 channels) |
| UARTs      | 8                 |
| SPIs       | 2                 |
| I2Cs       | 2                 |
| CAN        | 1                 |
| Vcc        | 2.0V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/web/en/resource/technical/document/datasheet/DM00115237.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/web/en/resource/technical/document/reference_manual/DM00031936.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/web/en/resource/technical/document/programming_manual/DM00051352.pdf) |
| Board Manual | [Board Manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00105823.pdf) |

### RIOT static pin mapping
please refer to [this](https://docs.google.com/spreadsheets/d/1gnxsux5WpFrn-33Ivb9nGgTBqooqgDYxRkhZms-cvsc/edit?usp=sharing)
document for the pin mapping as implemenented in `boards/nucleo-f091/include/periph_conf.h`

### User Interface
1 Button:

| NAME  | USER  |
|:----- |:----- |
| Pin   | PC13  |

1 LEDs:

| NAME   | LD2   |
| -----  | ----- |
| Color  | green |
| Pin    | PA5   |


## Implementation Status
| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F091RC   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | no        | |
|        | PWM       | no        | |
|        | UART      | full      | |
|        | I2C       | no        | |
|        | SPI       | no        | |
|        | USB       | no        | |
|        | Timer     | full      | |


## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-F091RC is `nucleo-f091rc`.
