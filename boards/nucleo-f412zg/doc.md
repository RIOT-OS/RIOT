 @defgroup    boards_nucleo-f412zg STM32 Nucleo-F412ZG
 @ingroup     boards_common_nucleo144
 @brief       Support for the STM32 Nucleo-F412ZG

## Overview

The Nucleo-F412ZG is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32F412ZG microcontroller with 256KiB of RAM and 1MiB of Flash.

## Pinout

@image html pinouts/nucleo-f412zg-and-f413zh.svg "Pinout for the Nucleo-F412ZG (from STM user manual, UM1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 34)" width=50%

### MCU

| MCU          |     STM32F412ZG
|:-------------|:--------------------|
| Family       | ARM Cortex-M4F      |
| Vendor       | ST Microelectronics |
| RAM          | 256KiB              |
| Flash        | 1MiB                |
| Frequency    | up to 100 MHz       |
| FPU          | yes                 |
| Timers       | 17 (2x watchdog, 1 SysTick, 2x 32-bit, 12x 16-bit) |
| ADC          | 1x 12 bit (up to 16 channels) |
| UARTs        | 4 (four USARTs)     |
| I2Cs         | 4                   |
| SPIs         | 5                   |
| CAN          | 2                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f412zg.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0402-stm32f412-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518-stm32-nucleo-144-boards-stmicroelectronics.pdf)|

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f412zg PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER requires ST-LINK firmware 2.37.26 or newer. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw
-link007.html).
