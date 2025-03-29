@defgroup    boards_nucleo-l152re STM32 Nucleo-L152RE
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-L152RE

## Overview

The Nucleo-L152RE is a board from ST's Nucleo family supporting a ARM
Cortex-M3 STM32L152RE microcontroller with 80KiB of RAM and 512KiB of Flash.

You can find general information about the Nucleo64 boards on the
@ref boards_common_nucleo64 page.

## Hardware

![Nucleo64 L152RE](http://www.open-electronics.org/wp-content/uploads/2015/08/Figura2-500x467.png)

## Pinout

@image html pinouts/nucleo-l152re.svg "Pinout for the Nucleo-L152RE (from STM user manual UM1724, http://www.st.com/resource/en/user_manual/dm00105823.pdf, page 34)" width=50%

### MCU
| MCU        | STM32L152RE       |
|:---------- |:----------------- |
| Family     | ARM Cortex-M3     |
| Vendor     | ST Microelectronics |
| RAM        | 80KiB             |
| Flash      | 512KiB            |
| EEPROM     | 16KB              |
| Frequency  | up to 32MHz       |
| FPU        | no                |
| Timers     | 12 (8x 16-bit, 1x 32-bit [TIM5], 1x Systick, 2x watchdog) |
| ADCs       | 1x 12-bit (21 channels) |
| UARTs      | 5                 |
| SPIs       | 3-8 (UARTs can be configured as SPIs) |
| I2Cs       | 2                 |
| Vcc        | 1.65V - 3.6V      |
| Datasheet  | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l152re.pdf) |
| Reference Manual | [Reference Manual](http://www.st.com/resource/en/reference_manual/cd00240193.pdf) |
| Programming Manual | [Programming Manual](http://www.st.com/web/en/resource/technical/document/programming_manual/CD00228163.pdf) |
| Board Manual | [Board Manual](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00105823.pdf) |

### User Interface

2 Buttons:

| NAME   | USER      | RESET |
|:------ |:--------- |:----- |
| Pin    | PC13 (IN) | NRST  |

1 LED:

| NAME   | LD2   |
| -----  | ----- |
| Color  | green |
| Pin    | PA5   |

## Flashing the Board

A detailed description about the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).
The board name for the Nucleo-L152RE is `nucleo-l152re`.

## Using UART

Using the UART isn't too stable right now. For some using the USB interface
just works, others have to connect the USB interface to a active USB hub and
others again can only transmit over the USB interface and receive using an
external UART adapter.

#### Connecting an external UART adapter

1. connect your usb tty to the st-link header as marked
![st-link-header](https://cloud.githubusercontent.com/assets/56618/5190200/f36aafe0-74e3-11e4-96bd-f755dd2a8b01.png)
2. done
