@defgroup    boards_nucleo-g071rb STM32 Nucleo-G071RB
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-G071RB

## Overview

The Nucleo-G071RB is a board from ST's Nucleo family supporting a ARM
Cortex-M0+ STM32G071RB microcontroller with 36KiB of RAM and 128KiB of Flash.

## Pinout

@image html pinouts/nucleo-g070rb-and-g071rb.svg "Pinout for the Nucleo-G071RG (from STM user manual UM2324, https://www.st.com/resource/en/user_manual/um2324-stm32-nucleo64-boards-mb1360-stmicroelectronics.pdf, page 34)" width=50%

### MCU

| MCU          | STM32G071RB
|:-------------|:--------------------|
| Family       | ARM Cortex-M0+      |
| Vendor       | ST Microelectronics |
| RAM          | 36KiB               |
| Flash        | 128KiB              |
| Frequency    | up to 64 MHz        |
| Timers       | 14 (2x watchdog, 1 SysTick, 10x 16-bit and 1 32-bit) |
| ADCs         | 1x 12 bit (up to 16 channels) |
| UARTs        | 5 (four USARTs and one Low-Power UART) |
| I2Cs         | 2                   |
| SPIs         | 2                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32g071rb.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0454-stm32g0x0-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um2324-stm32-nucleo64-boards-mb1360-stmicroelectronics.pdf)|

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:

```
make BOARD=nucleo-g071rb PROGRAMMER=cpy2remed flash
```

@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).
