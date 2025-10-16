@defgroup    boards_nucleo-g431rb STM32 Nucleo-G431RB
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-G431RB

## Overview

The Nucleo-G431RB is a board from ST's Nucleo family supporting a ARM
Cortex-M4 STM32G431RB microcontroller with 32KiB of RAM and 128KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Pinout

@image html pinouts/nucleo-g431rb-and-more.svg "Pinout for the Nucleo-G431RB (from STM user manual UM2505, https://www.st.com/resource/en/user_manual/um2505-stm32g4-nucleo64-boards-mb1367-stmicroelectronics.pdf, page 29)" width=50%

### MCU

| MCU          | STM32G431RB
|:-------------|:--------------------|
| Family       | ARM Cortex-M4       |
| Vendor       | ST Microelectronics |
| RAM          | 32KiB               |
| Flash        | 128KiB              |
| Frequency    | up to 170 MHz       |
| Timers       | 14 (2x watchdog, 1 SysTick, 10x 16-bit and 1 32-bit) |
| ADCs         | 2x 12 bit (up to 23 channels) |
| UARTs        | 5 (one UART, three USARTs and one Low-Power UART) |
| I2Cs         | 3                   |
| SPIs         | 3                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32g431rb.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2505-stm32g4-nucleo64-boards-mb1367-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-G431RB is `nucleo-g431rb`.
