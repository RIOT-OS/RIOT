/**
@defgroup    boards_bluepill-stm32f030c8 STM32F030C8 based Bluepill
@ingroup     boards
@brief       Support for the STM32F030C8 based Bluepill

## Overview

The STM32F030C8 based Bluepill is a very cheap breakout board for the STM32F030C8 MCU.

## Hardware

![STM32F030C8 based Bluepill](https://camo.githubusercontent.com/f33ec5f4068e1dcc4c549dbe3dc70d2ed784c19abf1c177f9640835180f88005/68747470733a2f2f696d616765732d6e612e73736c2d696d616765732d616d617a6f6e2e636f6d2f696d616765732f492f3631474765592532424c306a4c2e5f41435f534c313030305f2e6a7067)

### MCU
| MCU        | STM32F030C8T6         |
|:---------- |:--------------------- |
| Family     | ARM Cortex-M0         |
| Vendor     | ST Microelectronics   |
| RAM        | 8 KiB                 |
| Flash      | 64 KiB                |
| Frequency  | up to 48MHz           |
| FPU        | no                    |
| Timers     | 8 (2x watchdog, 1 SysTick, 5x 16-bit) |
| ADCs       | 1x 12-bit             |
| UARTs      | 2                     |
| SPIs       | 2                     |
| I2Cs       | 2                     |
| RTC        | 1                     |
| Vcc        | 2.0V - 3.6V           |
| Datasheet  | [Datasheet](https://www.st.com/en/microcontrollers-microprocessors/stm32f030f4.html) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/datasheet/stm32f030f4.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/resource/en/programming_manual/dm00051352.pdf) |

## Flashing the device

The STM32F030C8 based Bluepill board does not include a programmer.
You have to connect a separate ST-Link programmer to the (SW)DIO, (SW)CLK and GND
pins on the board.

If you want a serial terminal, you have to connect a separate USB-Serial adapter to
the PA09 (TX) and PA10 (RX) pins on the board.

The easiest way to program the board is to use OpenOCD. Once you have installed
OpenOCD (look [here](https://github.com/RIOT-OS/RIOT/wiki/OpenOCD) for
installation instructions), you can flash the board simply by typing

```
make BOARD=bluepill-stm32f030c8 flash
```
and debug via GDB by simply typing
```
make BOARD=bluepill-stm32f030c8 debug
```

## Known Issues

### Flashing fails

The board does not expose a pin for the reset signal but wires it only to the
reset button.
You can work around this by pressing the reset button when OpenOCD wants to connect
to the Blue Pill, or keep it pressed until OpenOCD tries to connect.
Hit the reset button again after flashing in order to boot the newly flashed image.

 */
