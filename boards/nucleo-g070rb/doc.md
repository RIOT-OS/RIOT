@defgroup    boards_nucleo-g070rb STM32 Nucleo-G070RB
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-G070RB

## Overview

The Nucleo-G070RB is a board from ST's Nucleo family supporting a ARM
Cortex-M0+ STM32G070RB microcontroller with 36KiB of RAM and 128KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Pinout

@image html pinouts/nucleo-g070rb-and-g071rb.svg "Pinout for the Nucleo-G070RG (from STM user manual UM2324, https://www.st.com/resource/en/user_manual/um2324-stm32-nucleo64-boards-mb1360-stmicroelectronics.pdf, page 34)" width=50%

### MCU

| MCU          | STM32G070RB
|:-------------|:--------------------|
| Family       | ARM Cortex-M0+      |
| Vendor       | ST Microelectronics |
| RAM          | 36KiB               |
| Flash        | 128KiB              |
| Frequency    | up to 64 MHz        |
| Timers       | 11 (2x watchdog, 1 SysTick, 8x 16-bit) |
| ADCs         | 1x 12 bit (up to 16 channels) |
| UARTs        | 4 USARTs            |
| I2Cs         | 2                   |
| SPIs         | 2                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32g070rb.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0454-stm32g0x0-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2324-stm32-nucleo64-boards-mb1360-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-G070RB is `nucleo-g070rb`.
