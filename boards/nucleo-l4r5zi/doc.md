/**
 * @defgroup    boards_nucleo144-l4r5 STM32 Nucleo-L4R5ZI
 * @ingroup     boards_common_nucleo144
 * @brief       Support for the STM32 Nucleo-L4R5ZI

## Overview

The Nucleo-L4R5ZI is a board from ST's Nucleo family supporting a ARM Cortex-M4
STM32L4R5ZI microcontroller with 640KiB of RAM and 2MiB of ROM Flash.

## Hardware

![Nucleo144 L4R5ZI](https://www.st.com/bin/ecommerce/api/image.PF264781.en.feature-description-include-personalized-no-cpn-large.jpg)

### MCU

| MCU          | STM32L4R5ZI                                        |
|:-------------|:---------------------------------------------------|
| Family       | ARM Cortex-M4F                                     |
| Vendor       | ST Microelectronics                                |
| RAM          | 640KiB                                             |
| Flash        | 2MiB                                               |
| Frequency    | up to 120 MHz                                      |
| FPU          | yes                                                |
| Timers       | 16 (2x watchdog, 1 SysTick, 2x 32-bit, 11x 16-bit) |
| ADC          | 1x 12 bit (16 channels)                            |
| UARTs        | 6                                                  |
| SPIs         | 3                                                  |
| I2Cs         | 4                                                  |
| CAN          | 1                                                  |
| USB          | 1                                                  |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l4r5zi.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0432-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/um2179-stm32-nucleo144-boards-mb1312-stmicroelectronics-1.pdf)|

## Flashing the device

### Flashing the Board Using OpenOCD

The ST Nucleo-L4R5ZI board includes an on-board ST-LINK programmer and can be
flashed using OpenOCD.
@note The latest release of OpenOCD doesn't contain support for this board,
so a recent development version (including  http://openocd.zylin.com/#/c/4777)
must be built from source to be able to flash this board.

To flash this board, just use the following command:

```
make BOARD=nucleo-l4r5zi flash -C examples/hello-world
```

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-l4r5zi PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Accessing RIOT shell

Default RIOT shell access utilize VCP (Virtual COM Port) via USB interface,
provided by integrated ST-LINK programmer. ST-LINK is connected to the
microcontroller LPUART1.

The default baud rate is 115 200.

Use the `term` target to open a terminal:

    make BOARD=nucleo-l4r5zi -C examples/hello-world term

 */
