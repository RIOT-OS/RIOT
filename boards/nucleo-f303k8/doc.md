@defgroup    boards_nucleo-f303k8 STM32 Nucleo-F303K8
@ingroup     boards_common_nucleo32
@brief       Support for the STM32 Nucleo-F303K8

## Overview

The Nucleo-F303K8 is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F303K8 microcontroller with 12KiB of RAM and 64KiB of ROM.

You can find general information about the Nucleo32 boards on the
@ref boards_common_nucleo32 page.

## Hardware

![nucleo image](https://www.st.com/bin/ecommerce/api/image.PF262496.en.feature-description-include-personalized-no-cpn-medium.jpg)


### MCU
| MCU        | STM32F303K8           |
|:---------- |:--------------------- |
| Family     | ARM Cortex-M4F        |
| Vendor     | ST Microelectronics   |
| RAM        | 12KiB                 |
| Flash      | 64KiB                 |
| Frequency  | up to 72MHz           |
| FPU        | yes                   |
| Timers     | 8 (4x 16-bit, 1x 32-bit [TIM2], 2x watchdog, 1x Systick) |
| ADCs       | 2x 12-bit (9 channels) |
| UARTs      | 2                     |
| SPIs       | 1                     |
| I2Cs       | 1                     |
| RTC        | 1                     |
| Vcc        | 2.0V - 3.6V           |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f303k8.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00043574.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/web/en/resource/technical/document/programming_manual/DM00046982.pdf) |
| Board Manual   | [Board Manual](https://www.st.com/resource/en/user_manual/dm00231744.pdf)|



## Implementation Status
| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F303K8   | partly    | Energy saving modes not fully utilized |
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
The board name for the Nucleo-F303K8 is `nucleo-f303k8`.
