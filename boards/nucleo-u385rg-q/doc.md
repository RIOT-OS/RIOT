@defgroup    boards_nucleo-u385rg-q STM32 Nucleo-U385RG-Q
@ingroup     boards_common_nucleo64
@brief       Support for the STM32 Nucleo-U385RG-Q

## Overview

The NUCLEO-U385RG-Q is a board from ST's Nucleo-64 family supporting the
STM32U385RG ultra-low-power microcontroller based on an ARM Cortex-M33 core
with TrustZone, 256KiB of RAM, and 1MiB of Flash.

You can find general information about the Nucleo-64 boards on the
@ref boards_common_nucleo64 page.

This RIOT port has moved past initial bring-up and now supports a range of the
MCU's peripherals (see the Supported Features section below).

## Hardware

### MCU

| MCU               | STM32U385RG          |
|:------------------|:---------------------|
| Family            | ARM Cortex-M33       |
| Vendor            | STMicroelectronics   |
| RAM               | 256KiB               |
| Flash             | 1MiB                 |
| Frequency         | up to 96MHz (currently clocked at 16MHz) |
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
STM32_Programmer_CLI -c port=SWD -w bin/nucleo-u385rg-q/<application>.elf -rst
@endcode

## Accessing RIOT shell

Default RIOT shell access uses the VCP (Virtual COM Port) over the USB interface
provided by the on-board ST-LINK programmer. The ST-LINK is connected to the
microcontroller's USART1 (`UART_DEV(0)`, STDIO).

The default baud rate is 115200.

If a physical connection to USART1 is needed, connect a UART interface to pins
PA9 (USART1 TX) and PA10 (USART1 RX).

## Supported Features

The peripherals below are configured for this board (see `Makefile.features`
and `include/periph_conf.h`). "Status" reflects whether the peripheral has been
verified on hardware:

| Peripheral | Status  | Configuration on this board                                  |
|:-----------|:--------|:-------------------------------------------------------------|
| GPIO       | working | including external interrupts (EXTI)                         |
| UART       | working | USART1 on PA9/PA10 (ST-LINK VCP, STDIO)                       |
| LPUART     | working | LPUART1 on PA2/PA3                                            |
| Timer      | working | TIM3 (general-purpose RIOT timer backend)                    |
| PWM        | working | TIM2 — CH1 User LED (PA5), CH2 PB3 (D3), CH3 PB10 (D6), CH4 PB11 |
| RTC        | working | uses on-board 32.768 kHz LSE                                 |
| SPI        | working | SPI1 on PA7/PA6/PA5 (MOSI/MISO/SCK), Arduino D11/D12/D13 — verified via loopback test |
| Flash page | working | program/erase + verify (VCORE voltage range 1)               |
| VBAT       | working | internal VBAT channel (scaling fixed)                        |
| ADC        | working | ADC1, Arduino A0–A5 plus internal VREFINT (verified with a potentiometer sweep) |
| I2C        | WIP     | I2C1 on PB8/PB9 (SCL/SDA), Arduino D15/D14 — bus scan can hang, under investigation |
| USB device | WIP     | USB DRD FS, D+/D− on PA12/PA11 — not yet functional, under investigation |

@note   I2C and USB are configured and compile, but are not yet working
        correctly on hardware (known bugs are being worked on). The remaining
        peripherals above have been confirmed working.

Arduino-style abstractions inherited from the common Nucleo-64 support are also
provided: `arduino_pins`, `arduino_analog`, `arduino_i2c`, `arduino_spi`,
`arduino_uart`, and `arduino_shield_uno`.

@note   TIM2 is reserved for PWM (User LED on TIM2_CH1/PA5), so TIM3 is used as
        the RIOT timer backend. PWM CH1 and SPI1 share the green User LED pin
        (PA5); avoid using both simultaneously.
