@defgroup    boards_nucleo144-l4r5 STM32 Nucleo-L4R5ZI
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-L4R5ZI

## Overview

The Nucleo-L4R5ZI is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32L4R5ZI microcontroller with 640KiB of RAM and 2MiB of ROM Flash.

You can find general information about the Nucleo144 boards on the
@ref boards_common_nucleo144 page.

## Hardware

![Nucleo144 L4R5ZI](https://www.st.com/bin/ecommerce/api/image.PF264781.en.feature-description-include-personalized-no-cpn-large.jpg)

## Pinout

@image html pinouts/nucleo-l4r5zi.svg "Pinout for the Nucleo-L4R5ZI (from STM user manual, UM2179, http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/um2179-stm32-nucleo144-boards-mb1312-stmicroelectronics.pdf, page 35)" width=55%

### MCU

| MCU          | STM32L4R5ZI                                        |
|:-------------|:---------------------------------------------------|
| Family       | ARM Cortex-M4F                                     |
| Vendor       | ST Microelectronics                                |
| RAM          | 640KiB                                             |
| Flash        | 2MiB                                               |
| Frequency    | up to 120 MHz                                      |
| FPU          | yes                                                |
| Timers       | 16 (2x watchdog, 1 SysTick, 2x 32-bit, 11x 16-bit) |
| ADC          | 1x 12 bit (16 channels)                            |
| UARTs        | 6                                                  |
| SPIs         | 3                                                  |
| I2Cs         | 4                                                  |
| CAN          | 1                                                  |
| USB          | 1                                                  |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l4r5zi.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0432-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/um2179-stm32-nucleo144-boards-mb1312-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-L4R5ZI is `nucleo-l4r5zi`.
