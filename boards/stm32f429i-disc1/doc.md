@defgroup    boards_stm32f429i-disc1 STM32F429I-DISC1
@ingroup     boards
@brief       Support for the STM32F429I-DISC1 board

## Overview

The STM32F429I-DISC1 is a board from ST's Discovery family supporting ARM
Cortex-M4 STM32F429ZI microcontroller with 256KiB of RAM and 2MiB of Flash.

This board is a replacement for the older @ref boards_stm32f429i-disco board.

## Pinout

<img src="stm32f429i-disc1.svg" alt="Pinout for the STM32F429I-DISC1, bottom layout (from STM user manual UM1607, https://www.st.com/resource/en/user_manual/um1670-discovery-kit-with-stm32f429zi-mcu-stmicroelectronics.pdf, page 12)" width=50% />

### MCU

| MCU          | STM32F429ZI         |
|:-------------|:--------------------|
| Family       | ARM Cortex-M4F      |
| Vendor       | ST Microelectronics |
| RAM          | 256KiB              |
| Flash        | 2MiB                |
| Frequency    | up to 180 MHz (set to 180MHz in RIOT) |
| RIOT default | 180 MHz             |
| FPU          | yes                 |
| Ethernet     | 10/100 Mbps         |
| Timers       | 17 (2x watchdog, 1 SysTick, 2x 32bit, 12x 16bit) |
| ADCs         | 3x 12 bit (up to 24 channels) |
| UARTs        | 4                   |
| I2Cs         | 3                   |
| SPIs         | 6                   |
| CAN          | 2                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f429zi.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um1670-discovery-kit-with-stm32f429zi-mcu-stmicroelectronics.pdf)|

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the STM32F429I-DISC1 is `stm32f429i-disc1`.
