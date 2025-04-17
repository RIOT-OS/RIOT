@defgroup    boards_nucleo-wl55jc STM32 Nucleo-WL55JC
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-W55JC

## Overview

The Nucleo-W55JC is a board from ST's Nucleo family supporting ARM dual-core
Cortex-M4/M0 STM ultra-low-power microcontroller with ultra-low-power LPWAN
radio solution, 64KiB of RAM and 256KiB of Flash.

### MCU


| MCU        |   STM32WL55JC                                             |
|:---------- |:--------------------------------------------------------- |
| Family     | ARM Cortex-M4 / ARM Cortex-M0+ Dual core                  |
| Vendor     | ST Microelectronics                                       |
| RAM        | 64KiB                                                     |
| Flash      | 256KiB                                                    |
| Frequency  | up to 48MHz                                               |
| FPU        | no                                                        |
| Timers     | 10 (2x watchdog, 1 SysTick, 6x 16-bit, 1x 32-bit [TIM2])  |
| ADCs       | 1x 12-bit (12 channel )                                   |
| UARTs      | 3 (two UARTs and one Low-Power UART)                      |
| SPIs       | 2                                                         |
| I2Cs       | 3                                                         |
| RTC        | 1                                                         |
| Vcc        | 1.8 V - 3.6V                                              |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32wl55jc.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00451556-stm32wl5x-advanced-armbased-32bit-mcus-with-subghz-radio-solution-stmicroelectronics.pdf) |
| Programming Manual 1 | [Reference Manual - M4 ](https://www.st.com/resource/en/programming_manual/dm00046982-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf) |
| Programming Manual 2 | [Reference Manual - M0](https://www.st.com/resource/en/programming_manual/dm00104451-cortexm0-programming-manual-for-stm32l0-stm32g0-stm32wl-and-stm32wb-series-stmicroelectronics.pdf) |
| Board Manual   | [Board Manual](https://www.st.com/resource/en/data_brief/nucleo-wl55jc.pdf) |
| Board Schematic | [Board Schematic](https://www.st.com/resource/en/user_manual/dm00622917-stm32wl-nucleo64-board-mb1389-stmicroelectronics.pdf) |
| Additional Reference | [Developing LoRaWAN Application](https://www.st.com/resource/en/application_note/dm00660451-how-to-build-a-lora-application-with-stm32cubewl-stmicroelectronics.pdf)

## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-wl55jc board includes an on-board ST-LINK V2 programmer. The
easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing

```
make BOARD=nucleo-wl55jc flash
```
and debug via GDB by simply typing
```
make BOARD=nucleo-wl55jc debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-wl55jc PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Supported Toolchains

For using the ST Nucleo-wl55jc board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
