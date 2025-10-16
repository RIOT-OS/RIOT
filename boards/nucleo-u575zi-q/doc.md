@defgroup    boards_nucleo-u575zi-q STM32 Nucleo-U575ZI-Q
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-U575ZI-Q

## Overview

The Nucleo-U575ZI-Q is a board from ST's Nucleo family supporting the ARM Cortex-M33
STM32U575ZIT6Q ultra-low-power microcontroller with TrustZone, 768KiB of RAM and 2MiB
of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![Nucleo144 U575ZI-Q](https://www.st.com/bin/ecommerce/api/image.PF271812.en.fea
ture-description-include-personalized-no-cpn-medium.jpg)

### MCU

| MCU          | STM32U575ZIT6Q               |
|:-------------|:-----------------------------|
| Family       | ARM Cortex-M33               |
| Vendor       | ST Microelectronics          |
| RAM          | 786KiB                       |
| Flash        | 2MiB                         |
| Frequency    | up tp 160MHz                 |
| FPU          | yes                          |
| TrustZone    | yes                          |
| Timers       | 17                           |
| UARTs        | 6 (3xUSART, 2xUART, 1xLPUART)|
| I2Cs         | 4                            |
| SPIs         | 3                            |
| CAN          | 1                            |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32u575zi.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0264-stm32-cortexm33-mcus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-U575ZI-Q is `nucleo-u575zi-q`.

## Accessing RIOT shell

Default RIOT shell access utilize VCP (Virtual COM Port) via USB interface,
provided by integrated ST-LINK programmer. ST-LINK is connected to the
microcontroller USART1.

The default baud rate is 115 200.

If a physical connection to USART1 is needed, connect UART interface to pins PA9 (USART1 TX) and PA10 (USART1 RX).
