@defgroup    boards_nucleo-l552ze-q STM32 Nucleo-L552ZE-Q
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-L552ZE-Q

## Overview

The Nucleo-L552ZE-Q is a board from ST's Nucleo family supporting ARM Cortex-M33
STM32L552ZE ultra-low-pawer microcontroller with TrustZone, 256KiB of RAM and 512KiB
of Flash.

You can find general information about the Nucleo144 boards on the
@ref boards_common_nucleo144 page.

## Hardware

![Nucleo144 L552ZE-Q](https://www.st.com/bin/ecommerce/api/image.PF267817.en.feature-description-include-personalized-no-cpn-large.jpg)

### MCU

| MCU          | STM32L552ZE                  |
|:-------------|:-----------------------------|
| Family       | ARM Cortex-M33               |
| Vendor       | ST Microelectronics          |
| RAM          | 256KiB                       |
| Flash        | 512KiB                       |
| Frequency    | up tp 110MHz                 |
| FPU          | yes                          |
| TrustZone    | yes                          |
| Timers       | 16                           |
| UARTs        | 6 (3xUSART, 2xUART, 1xLPUART)|
| I2Cs         | 4                            |
| SPIs         | 3                            |
| CAN          | 1                            |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l552ze.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00346336-stm32l552xx-and-stm32l562xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0264-stm32-cortexm33-mcus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00615305-stm32l5-nucleo-144-board-mb1361-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-L552ZE-Q is `nucleo-l552ze-q`.
