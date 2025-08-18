@defgroup    boards_nucleo-f446re STM32 Nucleo-F446RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-F446RE

## Overview

The Nucleo-F446RE is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32F446RE microcontroller with 128KiB of RAM and 512KiB of Flash.

## Hardware

![Nucleo64 F446RE](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-f446re.svg "Pinout for the Nucleo-F446RE (from STM user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 35)" width=50%

### MCU

| MCU        | STM32F446RE       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M4F    |
| Vendor     | ST Microelectronics |
| RAM        | 128KiB            |
| Flash      | 512KiB            |
| Frequency  | up to 180MHz      |
| FPU        | yes               |
| Timers     | 17 (2x watchdog, 1 SysTick, 12x 16-bit, 2x 32-bit [TIM2]) |
| ADCs       | 3x 12-bit (16 channels) |
| UARTs      | 6                 |
| SPIs       | 4                 |
| I2Cs       | 4                 |
| RTC        | 1                 |
| CAN        | 2                 |
| USB        | 1                 |
| Vcc        | 1.8V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f446re.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00135183.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/dm00046982.pdf) |
| Board Manual   | [Board Manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00105823.pdf)|

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F446RE   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (9 pins available)    |  |
|        | UART      | 3 UARTs       | USART2 via STLink/USB or D0(RX)/D1(TX), USART3 on PC11(RX)/PC10(TX) and USART1 on PA10(RX)/PA9(TX) |
|        | I2C       | one I2C       | |
|        | SPI       | one SPI       | |
|        | USB       | no        | |
|        | Timer     | one 32 bit timer      | |

## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-F446RE board includes an on-board ST-LINK V2 programmer. The
easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing

```
make BOARD=nucleo-f446re flash
```
and debug via GDB by simply typing
```
make BOARD=nucleo-f446re debug
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f446re PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Supported Toolchains

For using the ST Nucleo-F446RE board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
