/**
@defgroup    boards_alientek-pandora Alientek Pandora
@ingroup     boards
@brief       Support for the Alientek Pandora board

## Overview

The Alientek Pandora is a development board manufactured jointly by Alientek and RT-Thread company.
It is based on STM32L475VET6 and have on-board LCD, SD card and WiFi access.
Users may connect to its on board ST-Link 2.1 compatible debugger to flash or debug.

Documents of this board are available on its [Alientek official website](http://www.alientek.com/productinfo/716137.html).
It may require a translation software to read it in English.
Additional resources may be found on [RT-Thread bsp support page](https://gitee.com/rtthread/rt-thread/tree/master/bsp/stm32/stm32l475-atk-pandora).

### Flashing the board

To flash the board, use the on board ST-Link programmer/debugger.
Input the following command:

    make BOARD=alientek-pandora -C examples/hello-world flash

The NRST pin is connected to the on board debugger, so users do not need to reset manually
every time it requires to flash.

If the operating system raised a fault, users may manually reset the chip by pressing the reset button.
This button has a black color and is located between the four keys and the LED light.

### STDIO

STDIO is connected to pins PA9 (TX) and PA10 (RX).
Before you begin, check that the both the jumper caps marked as 'USART1' is connected.

Use the `term` target to open a terminal:

    make BOARD=alientek-pandora -C examples/hello-world term

An on-board ST-Link compatible debugger is used to transport serial STDIO message.

 */
