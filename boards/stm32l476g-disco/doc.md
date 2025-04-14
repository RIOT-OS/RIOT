@defgroup    boards_stm32l476g-disco STM32L476G-DISCO
@ingroup     boards
@brief       Support for the STM32L476G-DISCO board.

## Overview

The STM32L476GDISCOVERY is an evaluation board with the ARM Cortex-M4 based
ultra-low power microcontroller STM32L476VGT6 with 128KiB of RAM and 1MiB of
ROM Flash.

### Hardware

![STM32L476G-DISCO](https://www.st.com/bin/ecommerce/api/image.PF261635.en.feature-description-include-personalized-no-cpn-medium.jpg)

## Pinout

@image html pinouts/stm32l476g-disco.svg "Pinout for the stm32l476g-disco (from STM board manual)" width=50%

### MCU

| MCU        |   STM32L476VG      |
|:---------- |:------------------ |
| Family     | ARM Cortex-M4      |
| Vendor     | ST Microelectronics|
| RAM        | 128KiB             |
| Flash      | 1MiB               |
| Frequency  | up to 80MHz        |
| FPU        | yes                |
| Timers     | 16 (2x watchdog, 1 SysTick, 6x 16-bit, 2x 32-bit [TIM2])  |
| ADCs       | 3x 12-bit (up to 16 channels) |
| UARTs      | 6 (three USARTs, two UARTs and one Low-Power UART) |
| SPIs       | 3                  |
| I2Cs       | 3                  |
| RTC        | 1                  |
| CAN        | 1                  |
| Vcc        | 1.71 V - 3.6V      |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l476je.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/content/ccc/resource/technical/document/reference_manual/02/35/09/0c/4f/f7/40/03/DM00083560.pdf/files/DM00083560.pdf/jcr:content/translations/en.DM00083560.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf) |
| Board Manual   | [Board Manual](https://www.st.com/resource/en/user_manual/um1879-discovery-kit-with-stm32l476vg-mcu-stmicroelectronics.pdf) |

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=stm32l476g-disco PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
      can be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Accessing RIOT shell

Default RIOT shell access utilize VCP (Virtual COM Port) via USB interface,
provided by integrated ST-LINK programmer. ST-LINK is connected to the
microcontroller USART2.

The default baud rate is 115200.
