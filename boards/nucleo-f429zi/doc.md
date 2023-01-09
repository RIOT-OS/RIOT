/**
@defgroup    boards_nucleo-f429zi STM32 Nucleo-F429ZI
@ingroup     boards_common_nucleo144
@brief       Support for the STM32 Nucleo-F429ZI

## Overview

The Nucleo-F429ZI is a board from ST's Nucleo family supporting ARM Cortex-M4
STM32F429ZI microcontroller with 256KiB of RAM and 2MiB of Flash.

##Hardware

![Nucleo144 F429ZI](https://www.st.com/bin/ecommerce/api/image.PF262637.en.feature-description-include-personalized-no-cpn-medium.jpg)

### MCU

| MCU          | STM32F429ZI
|:-------------|:--------------------|
| Family       | ARM Cortex-M4       |
| Vendor       | ST Microelectronics |
| RAM          | 256KiB              |
| Flash        | 2MiB                |
| Frequency    | up to 180 MHz       |
| FPU          | yes                 |
| Ethernet     | 10/100 Mbps         |
| Timers       | 17                  |
| UARTs        | 4                   |
| I2cs         | 3                   |
| SPIs         | 6                   |
| CAN          | 2                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f429zi.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518-stm32-nucleo-144-boards-stmicroelectronics.pdf)|

## Flashing the device

### Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f429zi PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER requires ST-LINK firmware 2.37.26 or newer. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Accessing RIOT shell

Default RIOT shell access utilize VCP (Virtual COM Port) via USB interface,
provided by integrated ST-LINK programmer. ST-LINK is connected to the
microcontroller USART3.

The default baud rate is 115200.

If a physical connection to USART3 is needed, connect UART interface to pins
PD8 (USART3 TX) and PD9 (USART3 RX).

@note Accordingly to the [MCU Datasheet](https://www.st.com/resource/en/datasheet/stm32f429zi.pdf)
PD8 and PD9 pins are 5V tolerant (see table 10, page 62).

 */
