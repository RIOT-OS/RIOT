@defgroup    boards_nucleo-u385rg-q STM32 Nucleo-U385RG-Q
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-U385RG-Q

## Overview

The NUCLEO-U385RG-Q is a board from ST's Nucleo-64 family supporting the
STM32U385RG ultra-low-power microcontroller based on an ARM Cortex-M33 core
with TrustZone, 256KiB of RAM, and 1MiB of Flash.

You can find general information about the Nucleo-64 boards on the
@ref boards_common_nucleo64 page.

This is the initial bring-up of the STM32U3 family and the Nucleo-U385RG-Q
board: CPU/clock setup, GPIO, and the serial console (UART/LPUART). Additional
peripherals are added on top of this base (see [Supported Features](#supported-features)
below).

## Hardware

### MCU

| MCU               | STM32U385RG          |
|:------------------|:---------------------|
| Family            | ARM Cortex-M33       |
| Vendor            | STMicroelectronics   |
| RAM               | 256KiB               |
| Flash             | 1MiB                 |
| Frequency         | up to 96MHz          |
| FPU               | yes                  |
| TrustZone         | yes                  |
| Datasheet         | [Datasheet](https://www.st.com/resource/en/datasheet/stm32u375ce.pdf) |
| Reference Manual  | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0487-stm32u3-series-armbased-32bit-mcus-stmicroelectronics.pdf) |
| Programming Manual| [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0264-stm32-cortexm33-mcus-and-mpus-programming-manual-stmicroelectronics.pdf) |
| Board Manual      | [Board Manual](https://www.st.com/resource/en/user_manual/um3062-stm32u3u5-nucleo64-boards-mb1841-stmicroelectronics.pdf) |

## Flashing the Board

A detailed description of the flashing process can be found on the
[guides page](https://guide.riot-os.org/board_specific/stm32/).

The board name for the NUCLEO-U385RG-Q is `nucleo-u385rg-q`.

@note   OpenOCD does not yet support the STM32U3, so the default
        `make BOARD=nucleo-u385rg-q flash` (which uses OpenOCD) does not work
        for this board. Until OpenOCD support is available, build with
        `make BOARD=nucleo-u385rg-q` and flash the resulting ELF with the
        STM32CubeProgrammer CLI, e.g.:
@code
STM32_Programmer_CLI -c port=SWD mode=Normal reset=HWrst freq=8000 \
    -e all \
    -w bin/nucleo-u385rg-q/<application>.elf \
    -v -rst
@endcode

## Accessing RIOT shell

Default RIOT shell access uses the VCP (Virtual COM Port) over the USB interface
provided by the on-board ST-LINK programmer. ST-LINK is connected to the
microcontroller's USART1 (`UART_DEV(0)`, STDIO).

The default baud rate is 115200.

If a physical connection to USART1 is needed, connect a UART interface to pins
PA9 (USART1 TX) and PA10 (USART1 RX).

## Supported Features

The peripherals below are configured for this board (see `Makefile.features`
and `include/periph_conf.h`). "Status" reflects whether the peripheral has been
verified on hardware:

| Peripheral | Status   | Configuration on this board                                  |
|:-----------|:---------|:-------------------------------------------------------------|
| GPIO       | working  | including external interrupts (EXTI)                         |
| UART       | working  | USART1 on PA9/PA10 (ST-LINK VCP, STDIO)                       |
| LPUART     | working  | LPUART1 on PA2/PA3                                            |
| Timer      | working  | TIM3 (general-purpose RIOT timer backend)                    |
