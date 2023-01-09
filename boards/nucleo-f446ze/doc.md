/**
@defgroup    boards_nucleo-f446ze STM32 Nucleo-F446ZE
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F446ZE

## Overview

The Nucleo-F446ZE is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F446ZE microcontroller with 128KiB of RAM and 512KiB of ROM Flash.

## Hardware

![Nucleo144 F446ZE](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/nucleo144-board.png)

### MCU

| MCU        | STM32F446ZE       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4     |
| Vendor     | ST Microelectronics |
| RAM        | 128KiB            |
| Flash      | 512KiB            |
| Frequency  | up to 180MHz      |
| FPU        | yes               |
| Timers     | 17 (2x watchdog, 1 SysTick, 12x 16-bit, 2x 32-bit [TIM2]) |
| ADCs       | 3x 12-bit (24 channels) |
| UARTs      | 6                 |
| SPIs       | 4                 |
| I2Cs       | 4                 |
| RTC        | 1                 |
| CAN        | 2                 |
| USB        | 1                 |
| Vcc        | 1.8V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f446ze.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00135183.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/dm00046982.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518.pdf)|

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F446ZE   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (9 pins available)    |  |
|        | UART      | 3 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX), USART3 on PC11(RX)/PC10(TX) and USART1 on PA10(RX)/PA9(TX) |
|        | I2C       | one I2C       | |
|        | SPI       | one SPI       | |
|        | USB       | no        | |
|        | Timer     | one 32 bit timer      | |

## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-F446ZE board includes an on-board ST-LINK V2 programmer. The
easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing

```
make BOARD=nucleo-f446ze flash
```
and debug via GDB by simply typing
```
make BOARD=nucleo-f446ze debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f446ze PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER requires ST-LINK firmware 2.37.26 or newer. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Supported Toolchains

For using the ST Nucleo-F446ZE board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
 */
