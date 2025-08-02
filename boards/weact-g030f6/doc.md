@defgroup   boards_weact-g030f6 WeAct-G030F6 board
@ingroup    boards
@brief      Support for the WeAct-G030F6 board

## Overview

WeAct-G030F6 is a cheap little board based on the STM32G030F6P6 MCU.
It does not come with a programmer or USB connection, so an external
programmer (ST-Link, DAP-Link, etc) has to be used.

It is available on sites like AliExpress for less than 2€.

## Hardware

![WeAct-G030F6P4](https://github.com/RIOT-OS/RIOT/assets/1301112/d42a25e0-d331-4cae-ba48-1edfa01f35f9)

### MCU
| MCU              | STM32G030F6P6         |
|:---------------- |:--------------------- |
| Family           | ARM Cortex-M0+        |
| Vendor           | ST Microelectronics   |
| RAM              |  8KiB                 |
| Flash            | 32KiB                 |
| Frequency        | up to 64MHz           |
| FPU              | no                    |
| Timers           | 8 (2x watchdog, 1 SysTick, 5x 16-bit) |
| ADCs             | 1x 12-bit             |
| UARTs            | 2                     |
| SPIs             | 2                     |
| I2Cs             | 2                     |
| RTC              | 1                     |
| Vcc              | 2.0V - 3.6V           |
| Datasheet        | [Datasheet](https://www.st.com/resource/en/datasheet/stm32g030f6.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0454-stm32g0x0-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
