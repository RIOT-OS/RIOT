@defgroup    boards_nucleo-f413zh STM32 Nucleo-F413ZH
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F413ZH

## Overview

The Nucleo-F413ZH is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32F413ZH microcontroller with 320KiB of RAM and 1.5MiB of Flash.

## Pinout

@image html pinouts/nucleo-f412zg-and-f413zh.svg "Pinout for the nucleo-f413zh (from STM user manual, UM1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 34)" width=50%

### MCU

| MCU        |    STM32F413ZH      |
|:---------- |:------------------- |
| Family     | ARM Cortex-M4F      |
| Vendor     | ST Microelectronics |
| RAM        | 320KiB              |
| Flash      | 1.5MiB              |
| Frequency  | up to 100MHz        |
| FPU        | yes                 |
| Timers     | 18 (12x 16-bit, 2x 32, 1x RTC, 1x Systick, 2x Watchdog) |
| ADC        | 1x 12-bit (16 channels) |
| UARTs      | 6 (four USARTs and two UARTs) |
| SPIs       | 5                   |
| CANs       | 3                   |
| RTC        | 1                   |
| I2Cs       | 4                   |
| Vcc        | 1.7V - 3.6V         |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f413zh.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0430-stm32f413423-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518-stm32-nucleo-144-boards-stmicroelectronics.pdf) |

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f413zh PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER requires ST-LINK firmware 2.37.26 or newer. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).