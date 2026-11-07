<!--
SPDX-FileCopyrightText: 2025 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup    boards_esp32h2_devkit ESP32-H2-DevKitM-1 Board
@ingroup     boards_esp32h2
@brief       Support for generic ESP32-H2 boards
@author      Gunar Schorcht <gunar@schorcht.net>

\section esp32h2_devkit ESP32-H2-DevKitM-1

## Overview

The Espressif ESP32-H2-DevKitM-1 board uses the ESP32-H2-MINI-1x module.

Since the number of GPIOs and their possible uses on the ESP32-H2 are quite
limited, the ESP32-H2-DevKitM-1 should also work for most other ESP32-H2 boards.
Any modifications required for specific applications could be overridden by
\ref esp32_application_specific_configurations "application-specific board configuration".

<img src="https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32h2/_images/esp32-h2-devkitm-1_v1.2_callouts.png" alt="Espressif ESP32-H2-DevKitM-1" width=800px />

## Hardware

### MCU

Most features of the board are provided by the ESP32-H2 SoC. For detailed
information about the ESP32-H2 variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

### Board Configuration

ESP32-H2-DevKitM-1 boards have no special hardware on board with the exception
of a WS2812-compatible RGB-LED.

All GPIOs are simply broken out for flexibility. Therefore, the board
configuration is the most flexible one which provides:

- 5 x ADC channels at maximum
- 1 x SPI
- 1 x I2C
- 1 x UART
- 2 x PWM with 3 channels each
- 1 x RGB-LED

Since all GPIOs are broken out, GPIOs can be used for different purposes
in different applications. For flexibility, GPIOs can be listed in multiple
peripheral configurations, but they can only be used for one peripheral
at a time. For example, GPIO1, GPIO3, GPIO4 and GPIO5 can be used as ADC
channels or as signals for SPI_DEV(0) and PWM_DEV(1).

This is possible because GPIOs are only used for a specific peripheral
interface when either

- the corresponding peripheral module is used, i.e. `periph_spi`, or
- the corresponding init function is called, i.e. `adc_init`, or
- the corresponding init function is called, i.e. `pwm_init`

That is, the purpose for which a GPIO is used depends on which module
or function is used first.

For example, if module `periph_spi` is not used, the GPIOs listed in SPI
configuration can be used for the other purposes, that is, GPIO1, GPIO4 and
GPIO5 can be used as ADC channels or PWM channels.

The following table shows the default board configuration, which is sorted
according to the defined functionality of GPIOs. This configuration can be
overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | GPIOs  | Remarks |Configuration
:---------------|:-------|:--------|:----------------------------------
BUTTON0         | GPIO9  | | |
ADC             | GPIO1, GPIO2, GPIO3, GPIO4, GPIO5 | | see \ref esp32_adc_channels "ADC Channels"
PWM_DEV(0)      | GPIO2, GPIO12, GPIO22 | - | \ref esp32_pwm_channels "PWM Channels"
PWM_DEV(1)      | GPIO1, GPIO3, GPIO5 | - | \ref esp32_pwm_channels "PWM Channels"
I2C_DEV(0):SCL  | GPIO10 | | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0):SDA  | GPIO11 | | \ref esp32_i2c_interfaces "I2C Interfaces"
RGB-LED         | GPIO8 | supported by driver module `ws281x` | |
SPI_DEV(0):SCK  | GPIO4 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MISO | GPIO5 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MOSI | GPIO0 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):CS0  | GPIO1 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0):TxD | GPIO24 | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0):RxD | GPIO23 | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
USB D-            | GPIO26 | | |
USB D+            | GPIO26 | | |
</center>
<br>
@note
- The configuration of ADC channels contains all ESP32-H2 GPIOs that could
  be used as ADC channels.
- GPIO6 and GPIO7 are not broken out for any reason.
- GPPIO15 to GPIO21 are used for the in-package flash and are not broken out.

For detailed information about the peripheral configurations of ESP32-H2
boards, see section \ref esp32_peripherals "Common Peripherals".

### Board Pinout

The following figures show the pinout as configured by default board
definition.

<img src="https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32h2/_images/esp32-h2-devkitm-1-v1.2_pinlayout.png" alt="EPS32-H2-DevKitM-1 Pinout" />

The corresponding board schematics can be found:

- [ESP32-H2-DevKitM-1 v1.2](https://dl.espressif.com/dl/schematics/esp32-h2-devkitm-1_v1.2_schematics.pdf)
- [ESP32-H2-DevKitM-1 v1.3](https://dl.espressif.com/dl/schematics/esp32-h2-devkitm-1_v1.3_schematics.pdf)

## Flashing the Device

Flashing RIOT is quite easy. The board has a Micro-USB connector with
reset/boot/flash logic. Just connect the board to your host computer
and type using the programming port:

```shell
BOARD=esp32h2-devkit make flash ...
```

For detailed information about ESP32-H2 as well as configuring and compiling
RIOT for ESP32-H2 boards, see \ref esp32_riot.
