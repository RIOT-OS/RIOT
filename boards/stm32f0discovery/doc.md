@defgroup    boards_stm32f0discovery STM32F0Discovery
@ingroup     boards
@brief       Support for the STM32F0Discovery board

## Overview

The STM32F0Discovery is a board from ST's Discovery family supporting
ARM Cortex-M0 STM32F051 microcontroller with 8KiB of RAM and 64KiB of Flash.

## Pinout

<img src="stm32f0-disco.svg" alt="Pinout for the STM32F0Discovery (from STM user manual UM1525, https://www.st.com/resource/en/user_manual/um1525-stm32f0discovery-discovery-kit-for-stm32-f0-microcontrollers-stmicroelectronics.pdf, page 10)" width=50% />

## MCU

| MCU        |    STM32F051R8T6    |
|:---------- |:------------------- |
| Family     | ARM Cortex-M0       |
| Vendor     | ST Microelectronics |
| RAM        | 8KiB                |
| Flash      | 64KiB               |
| Frequency  | up to 48MHz (set to 48MHz on RIOT) |
| FPU        | no                  |
| Timers     | 11 (7x 16-bit, 1x 32, 1x Systick, 2x Watchdog) |
| ADC        | 1x 12-bit (16 channels) |
| UARTs      | 2 (two USARTs)      |
| SPIs       | 2                   |
| CANs       | no                  |
| RTC        | 1                   |
| I2Cs       | 2                   |
| Vcc        | 2.0V - 3.6V         |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f051r8.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0091-stm32f0x1stm32f0x2stm32f0x8-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0215-stm32f0-series-cortexm0-programming-manual-stmicroelectronics.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um1525-stm32f0discovery-discovery-kit-for-stm32-f0-microcontrollers-stmicroelectronics.pdf) |
