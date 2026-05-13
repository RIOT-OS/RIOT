@defgroup    boards_stm32f030f4-demo STM32F030F4 Demo Board
@ingroup     boards
@brief       Support for the STM32F030F4 Demo Board

## Overview

The STM32F030F4 Demo Board is a very cheap breakout board for the STM32F030F4 MCU.

## Hardware

![STM32F030F4 Demo Board](https://user-images.githubusercontent.com/20950920/48240567-e985c080-e3db-11e8-8775-68a216485b59.jpg)

### MCU

| MCU        | STM32F030R4           |
|:---------- |:--------------------- |
| Family     | ARM Cortex-M0         |
| Vendor     | ST Microelectronics   |
| RAM        | 4Kb                   |
| Flash      | 16Kb                  |
| Frequency  | up to 48MHz           |
| FPU        | no                    |
| Timers | 8 (2x watchdog, 1 SysTick, 5x 16-bit)    |
| ADCs       | 1x 12-bit             |
| UARTs      | 6                     |
| SPIs       | 1                     |
| I2Cs       | 1                     |
| RTC        | 1                     |
| Vcc        | 2.0V - 3.6V           |
| Datasheet  | [Datasheet](https://www.st.com/en/microcontrollers-microprocessors/stm32f030f4.html) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/datasheet/stm32f030f4.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/resource/en/programming_manual/dm00051352.pdf) |

## Flashing the device

The STM32F030F4 Demo Board board does not include a programmer.
You have to connect a separate ST-Link programmer to the (SW)DIO, (SW)CLK, GND and
NRST pins on the board.

If you want a serial terminal, you have to connect a separate USB-Serial adapter to
the RX and TX pins on the board.

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the STM32F030F4 Demo Board is `stm32f030f4-demo`.
