@defgroup    boards_nucleo-g474re STM32 Nucleo-G474RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-G474RE

## Overview

The Nucleo-G474RE is a board from ST's Nucleo family supporting a ARM
Cortex-M4 STM32G474RE microcontroller with 128KiB of RAM and 512KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Pinout

@image html pinouts/nucleo-g431rb-and-more.svg "Pinout for the Nucleo-G4741RE (from STM user manual UM2505, https://www.st.com/resource/en/user_manual/um2505-stm32g4-nucleo64-boards-mb1367-stmicroelectronics.pdf, page 29)" width=50%

### MCU

| MCU          | STM32G474RE
|:-------------|:--------------------|
| Family       | ARM Cortex-M4       |
| Vendor       | ST Microelectronics |
| RAM          | 128KiB              |
| Flash        | 512KiB              |
| Frequency    | up to 170 MHz       |
| Timers       | 17 (2x watchdog, 1 SysTick, 12x 16-bit and 2x 32-bit) |
| ADCs         | 5x 12 bit (up to 26 channels) |
| UARTs        | 5 (one UART, three USARTs and one Low-Power UART) |
| I2Cs         | 4                   |
| SPIs         | 3                   |
| CANs         | 3                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32g474re.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2505-stm32g4-nucleo64-boards-mb1367-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-G474RE is `nucleo-g474re`.
