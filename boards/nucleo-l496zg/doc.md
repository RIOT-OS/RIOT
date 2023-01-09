/**
@defgroup    boards_nucleo-l496zg STM32 Nucleo-L496ZG
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-l496zg

## Overview

The Nucleo-L496ZG is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32L496ZG ultra-low-pawer microcontroller with 320KiB of RAM and 1 MiB of Flash.

### MCU

| MCU        |   STM32L496ZG      |
|:---------- |:------------------ |
| Family     | ARM Cortex-M4F     |
| Vendor     | ST Microelectronics|
| RAM        | 320KiB             |
| Flash      | 1MiB               |
| Frequency  | up to 80MHz        |
| FPU        | yes                |
| Timers     | 16 (2x watchdog, 1 SysTick, 11x 16-bit, 2x 32-bit) |
| ADCs       | 3x 12-bit (24 channels |
| UARTs      | 6 (5x UARTs and one Low-Power UART) |
| SPIs       | 3                  |
| I2Cs       | 4                  |
| RTC        | 1                  |
| CAN        | 2                  |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l496vg.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0351-stm32l47xxx-stm32l48xxx-stm32l49xxx-and-stm32l4axxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/content/ccc/resource/technical/document/programming_manual/6c/3a/cb/e7/e4/ea/44/9b/DM00046982.pdf/files/DM00046982.pdf/jcr:content/translations/en.DM00046982.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2179-stm32-nucleo144-boards-mb1312-stmicroelectronics.pdf) |

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-l496zg PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
      can be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html

 */
