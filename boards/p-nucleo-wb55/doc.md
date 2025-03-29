@defgroup    boards_p-nucleo-wb55 STM32 P-Nucleo-WB55
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 P-Nucleo-WB55

## Overview

The p-nucleo-wb55 is a multi-protocol wireless and ultra-low-power
device embedding a powerful and ultra-low-power radio compliant with the
Bluetooth® Low Energy (BLE) SIG specification v5.0 and with IEEE
802.15.4-2011 board. It integrates an ARM Cortex-M4 STM32WB55RG microcontroller
with 256KB of RAM and 1MB of ROM Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![st-nucleo-wb55](https://miro.medium.com/max/700/1*9OG-4Ix4EzHX9uBpMve2IA.jpeg)

## Pinout

@image html pinouts/p-nucleo-wb55.svg "Pinout for the p-nucleo-wb55" width=50%

### MCU

| MCU               | STM32WB55RG                           |
|:----------------- |:------------------------------------- |
| Family            | ARM Cortex-M4                         |
| Vendor            | ST Microelectronics                   |
| RAM               | 256 KiB                               |
| Flash             | 512 KiB                               |
| Frequency         | 64 MHz                                |
| FPU               | yes                                   |
| Timers            | 8 (3x 16-bit, 1x 32-bit [TIM5])       |
| LPTimers          | 2x 16-bit                             |
| ADCs              | 1x 19-channel 12-bit                  |
| UARTs             | 1                                     |
| LUARTs            | 1                                     |
| SPIs              | 1                                     |
| I2Cs              | 2                                     |
| RTC               | 1                                     |
| Vcc               | 1.65V - 3.6V                          |
| Datasheet         | [Datasheet][Datasheet]                |
| Reference Manual  | [Reference Manual][Reference Manual]  |
| User Manual       | [User Manual][User Manual]            |

[Datasheet]: https://www.st.com/resource/en/datasheet/stm32wb55cc.pdf
[Reference Manual]: https://www.st.com/resource/en/reference_manual/rm0434-multiprotocol-wireless-32bit-mcu-armbased-cortexm4-with-fpu-bluetooth-lowenergy-and-802154-radio-solution-stmicroelectronics.pdf
[User Manual]: https://www.st.com/content/ccc/resource/technical/document/user_manual/group1/13/58/22/1a/f2/ff/43/5c/DM00517423/files/DM00517423.pdf/jcr:content/translations/en.DM00517423.pdf

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the P-Nucleo-WB55 is `p-nucleo-wb55`.

## User Interface

4 Button:

| NAME   | RESET/SW4 | SW1   | SW2   | SW3   |
|:------ |:--------- |:----- |:----- |:----- |
| Pin    | NRST      | PC4   | PD0   | PD1   |

3 LED:

| NAME   | LED1   | LED2   | LED3   |
| -----  | ------ | ------ | ------ |
| Color  | blue   | green  | red    |
| Pin    | PB5    | PB0    | PB1    |


## Implementation Status

| Device            | ID                | Supported | Comments   |
|:----------------- |:----------------- |:--------- |:---------- |
| MCU               | stm32wb           | yes       |            |
|                   | M0+ co-processor  | no        |            |
|                   | BLE               | no        |            |
|                   | 802.15.4          | no        |            |
| Low-level driver  | GPIO              | yes       |            |
|                   | UART              | yes       | USART1     |
|                   | LPUART            | yes       | LPUART1    |
|                   | I2C               | yes       | I2C1       |
|                   | SPI               | yes       |            |
|                   | ADC               | yes       |            |
|                   | RTT               | yes       |            |
|                   | RTC               | yes       |            |
|                   | RNG               | yes       |            |
|                   | Timer             | yes       | TIM2       |
|                   | WDT               | yes       |            |
|                   | USB               | yes       |            |
|                   | PWM               | yes       | dev 0/4 ch |
|                   | AES               | no        |            |
