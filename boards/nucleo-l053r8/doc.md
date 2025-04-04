@defgroup    boards_nucleo-l053r8 STM32 Nucleo-L053R8
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-L053R8

## Overview

The Nucleo-L053R8 is a board from ST's Nucleo family supporting a ARM
Cortex-M0+ STM32L053R8 microcontroller with 8KiB of RAM and 32KiB of Flash.

## Pinout

@image html pinouts/nucleo-l053r8.svg "Pinout for the Nucleo-L053r8 (from STM user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 33)" width=50%

### MCU

| MCU          | STM32L053R8
|:-------------|:--------------------|
| Family       | ARM Cortex-M0+      |
| Vendor       | ST Microelectronics |
| RAM          | 8KiB                |
| Flash        | 64KiB               |
| Frequency    | up to 32 MHz        |
| Timers       | 9 (2x watchdog, 1 SysTick, 5x 16-bit, 1x RTC) |
| ADCs         | 1x 12 bit (up to 16 channels) |
| UARTs        | 3 (two USARTs and one Low-Power UART) |
| I2Cs         | 2                   |
| SPIs         | 4                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l053c6.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0367-ultralowpower-stm32l0x3-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf)|


## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:

```
make BOARD=nucleo-l053r8 PROGRAMMER=cpy2remed flash
```

@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).
