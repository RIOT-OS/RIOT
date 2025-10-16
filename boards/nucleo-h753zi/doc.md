@defgroup    boards_nucleo-h753zi STM32 Nucleo-H753ZI
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-H753ZI

## Overview

The Nucleo-H753ZI is a board from ST's Nucleo family supporting ARM Cortex-M7
STM32H753ZI microcontroller with 1 MiB of RAM and 2 MiB of Flash.

## Pinout

"Pinout for the Nucleo-H753ZI (from STM user manual, UM2607, https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf, page 38)"

### MCU

| MCU          | STM32H753ZI |
|:-------------|:------------|
| Family       | ARM Cortex-M7 |
| Vendor       | ST Microelectronics |
| RAM          | 1 MiB       |
| Flash        | 2 MiB       |
| Frequency    | up to 480 MHz |
| FPU          | yes         |
| Ethernet     | 10/100 Mbps |
| Timers       | 22 (2x watchdog, 1 SysTick, 2x 32-bit, 17x 16-bit) |
| ADCs         | 3x 16 bit (up to 36 channels) |
| UARTs        | 4           |
| I2Cs         | 4           |
| SPIs         | 6           |
| CAN          | 2           |
| RTC          | 1           |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32h753zi.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0433-stm32h742-stm32h743753-and-stm32h750-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0253-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf) |
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-boards-mb1364-stmicroelectronics.pdf)|

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-h753zi flash term
```
