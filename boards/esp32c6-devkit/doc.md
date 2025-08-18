<!--
SPDX-FileCopyrightText: 2025 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup    boards_esp32c6_devkit ESP32-C6-DevKit Board
@ingroup     boards_esp32c6
@brief       Support for generic ESP32-C6 boards
@author      Gunar Schorcht <gunar@schorcht.net>

\section esp32c6_devkit ESP32-C6-DevKit

## Table of Contents {#esp32c6_devkit_toc}

-# [Overview](#esp32c6_devkit_overview)
-# [Hardware](#esp32c6_devkit_hardware)
    -# [MCU](#esp32c6_devkit_mcu)
    -# [Board Configuration](#esp32c6_devkit_board_configuration)
    -# [Board Pinout](#esp32c6_devkit_pinout)
-# [Flashing the Device](#esp32c6_devkit_flashing)

## Overview {#esp32c6_devkit_overview}

The Espressif ESP32-C6-DevKit boards are a couple of boards that use one of
the following modules:

- ESP32-C6-MINI-1x module (ESP32-C6-DevKitM-1 board)
- ESP32-C6-WROOM-1x module (ESP32-C6-DevKitC-1 board)

Since the number of GPIOs and their possible uses on the ESP32-C6 are quite
limited, the ESP32-C6-DevKit should also work for most other ESP32-C6 boards.
Any modifications required for specific applications could be overridden by
\ref esp32_application_specific_configurations "application-specific board configuration".

\image html "https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/_images/esp32-c6-devkitm-1-v1-annotated-photo.png" "Espressif ESP32-C6-DevKitM-1" width=800px

[Back to table of contents](#esp32c6_devkit_toc)

## Hardware {#esp32c6_devkit_hardware}

This section describes

- the [MCU](#esp32c6_devkit_mcu),
- the default [board configuration](#esp32c6_devkit_board_configuration),
- [optional hardware configurations](#esp32c6_devkit_optional_hardware),
- the [board pinout](#esp32c6_devkit_pinout).

[Back to table of contents](#esp32c6_devkit_toc)

### MCU {#esp32c6_devkit_mcu}

Most features of the board are provided by the ESP32-C6 SoC. For detailed
information about the ESP32-C6 variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

[Back to table of contents](#esp32c6_devkit_toc)

### Board Configuration {#esp32c6_devkit_board_configuration}

ESP32-C6-DevKit boards have no special hardware on board with the exception
of a WS2812-compatible RGB-LED.

All GPIOs are simply broken out for flexibility. Therefore, the board
configuration is the most flexible one which provides:

- 7 x ADC channels at maximum
- 1 x SPI
- 1 x I2C
- 1 x UART
- 2 x PWM with 3 channels each
- 1 x RGB-LED WS2812-compatible

Since all GPIOs are broken out, GPIOs can be used for different purposes
in different applications. For flexibility, GPIOs can be used in multiple
peripheral configurations, but they can only be used for one peripheral
at a time. For example, GPIO4 and GPIO5 are defined as ADC channels
and signals for I2C_DEV(0), GPIO7 and GPIO18 are defined as channels
for PWM_DEV(1) and signals for SPI_DEV(0).

This is possible because GPIOs are only used for a specific peripheral
interface when either

- the corresponding peripheral module is used, e.g. `periph_i2c` and
  `periph_spi`, or
- the corresponding init function is called, e.g. `adc_init` and
  `pwm_init`.

That is, the purpose for which a GPIO is used depends on which module
or function is used first. For example, if module `periph_i2c` is not used,
the GPIOs listed in I2C configuration can be used for the other purposes,
that is, GPIO4 and GPIO5 can be used as ADC channels.

The following table shows the default board configuration, which is sorted
according to the defined functionality of GPIOs. This configuration can be
overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | GPIOs  | Remarks |Configuration
:---------------|:-------|:--------|:----------------------------------
BUTTON0         | GPIO9  | | |
ADC             | GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5, GPIO6 | | see \ref esp32_adc_channels "ADC Channels"
PWM_DEV(0)      | GPIO19, GPIO20, GPIO21 | - | \ref esp32_pwm_channels "PWM Channels"
PWM_DEV(1)      | GPIO22, GPIO18, GPIO7  | - | \ref esp32_pwm_channels "PWM Channels"
I2C_DEV(0):SCL  | GPIO5  | | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0):SDA  | GPIO4  | | \ref esp32_i2c_interfaces "I2C Interfaces"
RGB-LED         | GPIO8  | supported by driver module `ws281x` | |
SPI_DEV(0):CLK  | GPIO6  | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MISO | GPIO2  | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MOSI | GPIO7  | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):CS0  | GPIO18 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0):TxD | GPIO16 | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0):RxD | GPIO17 | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
USB D-          | GPIO12 | | |
USB D+          | GPIO13 | | |
</center>
\n
@note
- The configuration of ADC channels contains all ESP32-C6 GPIOs that could
  be used as ADC channels.
- GPIO10, GPIO11 and GPIO24 to GPIO30 are used for internal flash and are not broken out.
- GPIO0 and GPIO1 can be used to connect an external 32.678 kHz crystal.
- GPIO15 is a strapping pin used to select the JTAG interface if
  `JTAG_SEL_ENABLE` is burned in the eFuses.

For detailed information about the peripheral configurations of ESP32-C6
boards, see section \ref esp32_peripherals "Common Peripherals".

[Back to table of contents](#esp32c6_devkit_toc)

### Board Pinout {#esp32c6_devkit_pinout}

The following figures show the pinouts as configured by default board
definition.

@image html https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/_images/esp32-c6-devkitm-1-pin-layout.png "EPS32-C6-DevKitM-1x Pinout"
@image html https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/_images/esp32-c6-devkitc-1-pin-layout.png "EPS32-C6-DevKitC-1x Pinout"

The corresponding board schematics can be found:

- [ESP32-C6-DevKitM-1](https://dl.espressif.com/dl/schematics/esp32-c6-devkitm-1-schematics.pdf)
- [ESP32-C6-DevKitC-1](https://dl.espressif.com/dl/schematics/esp32-c6-devkitc-1-schematics_v1.4.pdf)

[Back to table of contents](#esp32c6_devkit_toc)

## Flashing the Device {#esp32c6_devkit_flashing}

Flashing RIOT is quite easy. The board has a Micro-USB connector with
reset/boot/flash logic. Just connect the board to your host computer
using the programming port and execute command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BOARD=esp32c6-devkit make flash ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For detailed information about ESP32-C6 as well as configuring and compiling
RIOT for ESP32-C6 boards, see \ref esp32_riot.

[Back to table of contents](#esp32c6_devkit_toc)
