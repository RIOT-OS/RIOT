@defgroup    boards_nucleo-l412kb STM32 Nucleo-L412KB
@ingroup     boards_common_nucleo32
@brief       Support for the STM32 Nucleo-L412KB

## Overview

The Nucleo-L412KB is a board from ST's Nucleo family supporting ARM-Cortex-M4
STM32L412KB microcontroller with 40KiB of RAM and 128KiB of Flash.

## Pinout

@image html pinouts/nucleo-l432kc-and-more.svg "Pinout for the Nucleo-L412KB (from STM user manual, UM1956, https://www.st.com/resource/en/user_manual/um1956-stm32-nucleo32-boards-mb1180-stmicroelectronics.pdf, page 32)" width=25%

### MCU

| MCU        |    STM32L412KB      |
|:---------- |:------------------- |
| Family     | ARM Cortex-M4F      |
| Vendor     | ST Microelectronics |
| RAM        | 40KiB               |
| Flash      | 128KiB              |
| Frequency  | up to 80MHz         |
| FPU        | yes                 |
| Timers     | 10 (6x 16-bit, 1x 32, 1x Systick, 2x Watchdog) |
| ADC        | 2x 12-bit (10 channels) |
| UARTs      | 4 (two USARTs and one Low-Power UART) |
| SPIs       | 1                   |
| CANs       | 0                   |
| RTC        | 1                   |
| I2Cs       | 2                   |
| Vcc        | 1.7V - 3.6V         |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l412c8.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0394-stm32l41xxx42xxx43xxx44xxx45xxx46xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](http://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um1956-stm32-nucleo32-boards-mb1180-stmicroelectronics.pdf) |

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-l412kb PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
      can be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).
