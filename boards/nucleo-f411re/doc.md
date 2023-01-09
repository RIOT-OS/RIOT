/**
@defgroup    boards_nucleo-f411re STM32 Nucleo-F411RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F411RE

## Overview

The Nucleo-F411RE is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F411RE microcontroller with 128KiB of RAM and 512KiB of Flash.

## Hardware

![Nucleo64 F411RE](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

### MCU

| MCU        | STM32F411RE       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4     |
| Vendor     | ST Microelectronics |
| RAM        | 128KiB            |
| Flash      | 512KiB            |
| Frequency  | up to 100MHz      |
| FPU        | yes               |
| Timers     | 11 (2x watchdog, 1 SysTick, 7x 16-bit, 1x 32-bit [TIM2]) |
| ADCs       | 1x 12-bit (16 channels) |
| UARTs      | 3                 |
| SPIs       | 5                 |
| I2Cs       | 3                 |
| RTC        | 1                 |
| USB        | 1                 |
| Vcc        | 1.7V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f411re.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/resource/en/reference_manual/dm00119316.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/resource/en/programming_manual/dm00046982.pdf) |
| Board Manual | [Board Manual](http://www.st.com/resource/en/user_manual/dm00105823.pdf) |

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F411RE   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (7 pins available))   |  |
|        | UART      | 3 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX), USART1 on PA10(RX)/PA9(TX) and USART6 on PA12(RX)/PA11(TX) |
|        | I2C       | 1 I2C     | |
|        | SPI       | 1 SPI     | |
|        | USB       | no        | |
|        | Timer     | 1 32 bit timer (TIM5)     | |

## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-F411RE board includes an on-board ST-LINK V2-1 programmer. The
easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing

```
make BOARD=nucleo-f411re flash
```
and debug via GDB by simply typing
```
make BOARD=nucleo-f411re debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f411re PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).


## Supported Toolchains

For using the ST Nucleo-F411RE board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
 */
