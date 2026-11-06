@defgroup    boards_nucleo-g491re STM32 Nucleo-G491RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-G491RE

## Overview

The Nucleo-G491RE is a board from ST's Nucleo family supporting a ARM
Cortex-M4 STM32G491RE microcontroller with 112KiB of RAM and 512KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Pinout

<img src="nucleo-g431rb-and-more.svg" alt="Pinout for the Nucleo-G491RE (from STM user manual UM2505 Rev 7, https://www.st.com/resource/en/user_manual/um2505-stm32g4-nucleo64-boards-mb1367-stmicroelectronics.pdf, page 30)" width=50% />

### MCU

| MCU          | STM32G491RE
|:-------------|:--------------------|
| Family       | ARM Cortex-M4       |
| Vendor       | ST Microelectronics |
| RAM          | 112KiB              |
| Flash        | 512KiB              |
| Frequency    | up to 170MHz, clocked to 170MHz in RIOT |
| Timers       | 15 (2x watchdog, 1 SysTick, 10x 16-bit, 1x 32-bit and 1x low power) |
| ADCs         | 3x 16 bit (up to 36 channels) |
| UARTs        | 6 (two UART, three USARTs and one Low-Power UART) |
| I2Cs         | 3                   |
| SPIs         | 3                   |
| FDCANs       | 2                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32g491cc.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2505-stm32g4-nucleo64-boards-mb1367-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-G491RE is `nucleo-g491re`.
