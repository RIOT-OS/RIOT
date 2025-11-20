@defgroup    boards_stm32mp157c-dk2 STM32MP157C-DK2 board
@ingroup     boards
@brief       Support for the STM32MP157C-DK2 board

## Overview

The STM32MP157C-DK2 is a board from ST featuring a double architecture based on
a dual Cortex-A7 and a Cortex-M4 STM32MP157C microcontroller with 384KB of SRAM
and no ROM Flash.

## Hardware

![STM32MP157C-DK2](https://www.st.com/bin/ecommerce/api/image.PF267415.en.feature-description-include-personalized-no-cpn-medium.jpg)

### MCU

| MCU        | STM32MP157CAC     |
|:---------- |:----------------- |
| Family     | ARM Dual Cortex-A7 & Cortex-M4 |
| Vendor     | ST Microelectronics |
| RAM        | 384Kb for Cortex-M4 |
| Flash      | None but 64KB of RETRAM |
| Frequency  | up to 209MHz      |
| FPU        | yes               |
| Timers     | 32 (3x watchdog, 2x 4 Cortex-A7 system timers, 1x SysTick, 5x 16-bit Low-Power, 12x 16-bit, 2x 32-bit, 1 RTC) |
| ADCs       | 2x 12-bit (16 channels), 2x 16-bit (16 channels) |
| UARTs      | 4x UART + 4x USART |
| SPIs       | 6                 |
| I2Cs       | 6                 |
| RTC        | 1                 |
| CAN        | 2                 |
| USB        | 3                 |
| Vcc        | 1.8V - 3.6V       |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32mp157c.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00327659-stm32mp157-advanced-armbased-32bit-mpus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/dm00046982-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf) |
| Board Manual   | [Board Manual](https://www.st.com/resource/en/user_manual/dm00591354-discovery-kits-with-stm32mp157-mpus-stmicroelectronics.pdf)|

## Implementation Status

| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32MP157CAC   | partly    | |
| Low-level driver | GPIO    | yes       | |
|        | UART      | 1 UART       | USART3 on PB12(RX)/PB10(TX) |
|        | Timer     | one 32 bit timer  | TIM2 |

## Flashing the device

Note that the STM32MP157C-DK2 board has no ROM Flash, thus the firmware
needs to be reflashed each time the board is rebooted.

### Boot selection jumper:

| BOOT mode | BOOT0 | BOOT2 |
| :-------- |:----- |:----- |
| Engineering |  1  |   1   |
| SD Card (Linux) |  0  |   1   |

### Engineering mode

The STM32MP157C-DK2 board includes an on-board ST-LINK V2 programmer. The
easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing inside
your application directory:

```
USEMODULE='stm32mp1_eng_mode' make BOARD=stm32mp157c-dk2 flash
```
and debug via GDB by simply typing
```
USEMODULE='stm32mp1_eng_mode' make BOARD=stm32mp157c-dk2 debug
```
### SD Card (Linux) mode

This assumes that Linux is booted and that your Linux kernel supports STM32
remoteproc framework.

Build the firmware inside your application directory:
```
make BOARD=stm32mp157c-dk2
```
Copy your firmware firmware.elf in /lib/firmwares on the Linux system.
(replace firmware.elf by your firmware filename)

Then simply launch this commands on the Linux system as root user:
```
echo firmware.elf > /sys/class/remoteproc/remoteproc0/firmware
echo start > /sys/class/remoteproc/remoteproc0/state
```

You can stop RIOT from Linux command line:
```
echo stop > /sys/class/remoteproc/remoteproc0/state
```

## Supported Toolchains

For using the STM32MP157C-DK2 board we strongly recommend the usage of the
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded)
toolchain.
