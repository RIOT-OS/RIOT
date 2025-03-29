@defgroup    boards_nucleo-g431rb STM32 Nucleo-G431RB
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-G431RB

## Overview

The Nucleo-G431RB is a board from ST's Nucleo family supporting a ARM
Cortex-M4 STM32G431RB microcontroller with 32KiB of RAM and 128KiB of Flash.

## Pinout

@image html pinouts/nucleo-g431rb-and-more.svg "Pinout for the Nucleo-G431RB (from STM user manual UM2505, https://www.st.com/resource/en/user_manual/um2505-stm32g4-nucleo64-boards-mb1367-stmicroelectronics.pdf, page 29)" width=50%

### MCU

| MCU          | STM32G431RB
|:-------------|:--------------------|
| Family       | ARM Cortex-M4       |
| Vendor       | ST Microelectronics |
| RAM          | 32KiB               |
| Flash        | 128KiB              |
| Frequency    | up to 170 MHz       |
| Timers       | 14 (2x watchdog, 1 SysTick, 10x 16-bit and 1 32-bit) |
| ADCs         | 2x 12 bit (up to 23 channels) |
| UARTs        | 5 (one UART, three USARTs and one Low-Power UART) |
| I2Cs         | 3                   |
| SPIs         | 3                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32g431rb.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2505-stm32g4-nucleo64-boards-mb1367-stmicroelectronics.pdf)|

## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-G431RB board includes an on-board ST-LINK V3 programmer. The
easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing

```
make BOARD=nucleo-g431rb flash
```
and debug via GDB by simply typing
```
make BOARD=nucleo-g431rb debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-g431re PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).


## Supported Toolchains

For using the ST Nucleo-G431RB board we recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
