/**
@defgroup    boards_olimexino-stm32 STM32 Olimexino-stm32
@ingroup     boards
@brief       Support for the Olimexino STM32 board

## Overview

The Olimexino-stm32 is a board from Olimexino family supporting a ARM Cortex-M3
STM32F103RB microcontroller with 20Kb of SRAM and 128Kb of ROM Flash.

## Hardware

![Olimexino STM32](https://www.olimex.com/Products/Duino/STM32/OLIMEXINO-STM32/images/thumbs/310x230/OLIMEXINO-STM32-01.jpg)

### MCU
| MCU        | STM32F103RB       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M3     |
| Vendor     | ST Microelectronics |
| RAM        | 20Kb              |
| Flash      | 128Kb             |
| Frequency  | up to 72MHz       |
| FPU        | no                |
| Timers     | 7 (2x watchdog, 1 SysTick, 4x 16-bit) |
| ADCs       | 1x 12-bit (16 channels) |
| UARTs      | 3                 |
| SPIs       | 2                 |
| I2Cs       | 2                 |
| RTC        | 1                 |
| USB        | 1                 |
| CAN        | 1                 |
| Vcc        | 2.0V - 3.6V       |
| Datasheet  | [Datasheet](http://www.st.com/resource/en/datasheet/stm32f103rb.pdf) |
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190.pdf) |
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/cd00228163.pdf) |
| Board Manual | [Board Manual](https://www.olimex.com/Products/Duino/STM32/OLIMEXINO-STM32/resources/OLIMEXINO-STM32.pdf) |

## Implementation Status
| Device | ID        | Supported | Comments  |
|:------------- |:------------- |:------------- |:------------- |
| MCU        | STM32F103RB   | partly    | Energy saving modes not fully utilized |
| Low-level driver | GPIO    | yes       | |
|        | PWM       | yes (4 pins available)  |  |
|        | UART      | 3 UARTs       | USART2 via D0(RX)/D1(TX), USART1 on PA10(RX)/PA09(TX) and USART3 on PB11(RX)/PB10(TX) |
|        | ADC       | 6 ADCs       | |
|        | I2C       | yes (I2C1 and I2C2)       | |
|        | SPI       | yes (SPI1 and SPI2)       | |
|        | USB       | no        | |
|        | Timer     | 3 16 bit timers (TIM2, TIM3 and TIM4)       | |

## Flashing the device
The Olimexino-stm32 needs to be flashed using stm32flash (repo [here](https://github.com/stm32duino/stm32flash)).
Once you have installed the program, you can flash the board like this:

```
make BOARD=olimexino-stm32 flash
```
and open a terminal using:
```
make BOARD=olimexino-stm32 term
```

USART1 is used for flashing the board whereas USART2 is the serial Output.

## Supported Toolchains
For using the OLIMEXINO STM32 board you need to use ```arm-none-eabi```.
 */
