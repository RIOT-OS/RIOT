@defgroup    boards_nucleo-f303ze STM32 Nucleo-F303ZE
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F303ZE

## Overview

The Nucleo-F303ZE is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F303ZE microcontroller with 64KiB of RAM and 512KiB of ROM.

## Hardware

![Nucleo144 F303ZE](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/nucleo144-board.png)

## Pinout

@image html pinouts/nucleo-f303ze.svg "Pinout for the Nucleo-F303ZE (from STM user board manual, M1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 33)" width=55%

### MCU
| MCU        | STM32F303ZE       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4F    |
| Vendor     | ST Microelectronics |
| RAM        | 64KiB             |
| Flash      | 512KiB            |
| Frequency  | up to 72MHz       |
| FPU        | yes               |
| Timers     | 14 (10x 16-bit, 1x 32-bit [TIM2], 1x Systick, 2x watchdog) |
| ADCs       | 4x 12-bit (40 channels) |
| UARTs      | 5                 |
| SPIs       | 4                 |
| I2Cs       | 3                 |
| RTC        | 1                 |
| CAN        | 1                 |
| USB        | 1                 |
| Vcc        | 2.0V - 3.6V       |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f303ze.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/web/en/resource/technical/document/reference_manual/DM00043574.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/web/en/resource/technical/document/programming_manual/DM00046982.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518.pdf) |



## Implementation Status
| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F303ZE   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | one PWM   | |
|        | UART      | three UART    | |
|        | I2C       | two I2C   | |
|        | SPI       | two SPI   | |
|        | USB       | no        | |
|        | Timer     | one 32 timer  | |


## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-F303ZE board includes an on-board ST-LINK V2 programmer.
The easiest way to program the board is to use OpenOCD. Once you have
installed OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD)
for installation instructions), you can flash the board simply by typing.

```
BOARD=nucleo-f303ze make flash
```
and debug via GDB by simply typing
```
BOARD=nucleo-f303ze make debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f303ze PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).


## Supported Toolchains
For using the ST Nucleo-F303ZE board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
