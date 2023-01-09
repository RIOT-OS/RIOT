/**
@defgroup    boards_nucleo-f091rc STM32 Nucleo-F091RC
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F091RC

## Overview

The Nucleo-F091RC is a board from ST's Nucleo family supporting a ARM Cortex-M0
STM32F091RC microcontroller with 32KiB of RAM and 256KiB of Flash.

## Hardware

![Nucleo64 F091RC](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)
![nucleo-f091rc pinout](https://raw.githubusercontent.com/wiki/RIOT-OS/RIOT/images/nucleo-f091_pinout.png)


### MCU
| MCU        | STM32F091RC       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M0     |
| Vendor     | ST Microelectronics |
| RAM        | 32KiB             |
| Flash      | 256KiB            |
| Frequency  | up to 48MHz       |
| FPU        | no                |
| Timers     | 12 (8x 16-bit, 1x 32-bit [TIM2], 1x Systick, 2x watchdog) |
| ADCs       | 1x 12-bit (up to 16 channels) |
| UARTs      | 8                 |
| SPIs       | 2                 |
| I2Cs       | 2                 |
| CAN        | 1                 |
| Vcc        | 2.0V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/web/en/resource/technical/document/datasheet/DM00115237.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/web/en/resource/technical/document/reference_manual/DM00031936.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/web/en/resource/technical/document/programming_manual/DM00051352.pdf) |
| Board Manual | [Board Manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00105823.pdf) |

### RIOT static pin mapping
please refer to [this](https://docs.google.com/spreadsheets/d/1gnxsux5WpFrn-33Ivb9nGgTBqooqgDYxRkhZms-cvsc/edit?usp=sharing)
document for the pin mapping as implemenented in `boards/nucleo-f091/include/periph_conf.h`

### User Interface
1 Button:

| NAME  | USER  |
|:----- |:----- |
| Pin   | PC13  |

1 LEDs:

| NAME   | LD2   |
| -----  | ----- |
| Color  | green |
| Pin    | PA5   |


## Implementation Status
| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F091RC   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | no        | |
|        | PWM       | no        | |
|        | UART      | full      | |
|        | I2C       | no        | |
|        | SPI       | no        | |
|        | USB       | no        | |
|        | Timer     | full      | |


## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-F091RC board includes an on-board ST-LINK V2 programmer. The
easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing

```
make BOARD=nucleo-f091rc flash
```
and debug via GDB by simply typing
```
make BOARD=nucleo-f091rc debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f091rc PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Supported Toolchains
For using the ST Nucleo-F091RC board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
 */
