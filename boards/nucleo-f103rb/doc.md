@defgroup    boards_nucleo-f103rb STM32 Nucleo-F103RB
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F103RB

## Overview

The Nucleo-F103RB is a board from ST's Nucleo family supporting a ARM Cortex-M3
STM32F103RB microcontroller with 20KiB of RAM and 128KiB of Flash.

## Hardware

![Nucleo64 F103RB](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-f103rb.svg "Pinout for the Nucleo-F103RB (from user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 30)" width=50%

### MCU
| MCU        | STM32F103RB       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M3     |
| Vendor     | ST Microelectronics |
| RAM        | 20KiB             |
| Flash      | 128KiB            |
| Frequency  | up to 72MHz       |
| FPU        | no                |
| Timers     | 7 (2x watchdog, 1 SysTick, 4x 16-bit) |
| ADCs       | 1x 12-bit (16 channels) |
| UARTs      | 3                 |
| SPIs       | 2                 |
| I2Cs       | 2                 |
| RTC        | 1                 |
| USB        | 1                 |
| CAN        | 1                 |
| Vcc        | 2.0V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f103rb.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/cd00228163.pdf) |
| Board Manual | [Board Manual](http://www.st.com/resource/en/user_manual/dm00105823.pdf) |

## Implementation Status
| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F103RB   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (4 pins available)  |  |
|        | UART      | 2 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX), USART1 on PA10(RX)/PA09(TX) and USART3 on PB11(RX)/PB10(TX) |
|        | ADC       | yes       | Pins A0 - A5 |
|        | I2C       | yes (I2C1 and I2C2)       | |
|        | SPI       | yes (SPI1 and SPI2)       | |
|        | USB       | no        | |
|        | Timer     | 2 16 bit timers (TIM2 and TIM3)       | |

## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-F103RB board includes an on-board ST-LINK V2 programmer. The
easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing:

```
make BOARD=nucleo-f103rb flash
```
and debug via GDB by simply typing
```
make BOARD=nucleo-f103rb debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f103rb PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Supported Toolchains
For using the ST Nucleo-F103RB board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
