<!--
Copyright (C) 2025 Gunar Schorcht

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
-->

@defgroup    boards_esp32h2_devkit ESP32-H2-DevKitM-1 Board
@ingroup     boards_esp32h2
@brief       Support for generic ESP32-H2 boards
@author      Gunar Schorcht <gunar@schorcht.net>

\section esp32h2_devkit ESP32-H2-DevKitM-1

## Table of Contents {#esp32h2_devkit_toc}

-# [Overview](#esp32h2_devkit_overview)
-# [Hardware](#esp32h2_devkit_hardware)
    -# [MCU](#esp32h2_devkit_mcu)
    -# [Board Configuration](#esp32h2_devkit_board_configuration)
    -# [Board Pinout](#esp32h2_devkit_pinout)
-# [Flashing the Device](#esp32h2_devkit_flashing)

## Overview {#esp32h2_devkit_overview}

The Espressif ESP32-H2-DevKitM-1 board uses the ESP32-H2-MINI-1x module.

Since the number of GPIOs and their possible uses on the ESP32-H2 are quite
limited, the ESP32-H2-DevKitM-1 should also work for most other ESP32-H2 boards.
Any modifications required for specific applications could be overridden by
\ref esp32_application_specific_configurations "application-specific board configuration".

\image html "https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32h2/_images/esp32-h2-devkitm-1_v1.2_callouts.png" "Espressif ESP32-H2-DevKitM-1" width=800px

[Back to table of contents](#esp32h2_devkit_toc)

## Hardware {#esp32h2_devkit_hardware}

This section describes

- the [MCU](#esp32h2_devkit_mcu),
- the default [board configuration](#esp32h2_devkit_board_configuration),
- [optional hardware configurations](#esp32h2_devkit_optional_hardware),
- the [board pinout](#esp32h2_devkit_pinout).

[Back to table of contents](#esp32h2_devkit_toc)

### MCU {#esp32h2_devkit_mcu}

Most features of the board are provided by the ESP32-H2 SoC. For detailed
information about the ESP32-H2 variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

[Back to table of contents](#esp32h2_devkit_toc)

### Board Configuration {#esp32h2_devkit_board_configuration}

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
\n
@note
- The configuration of ADC channels contains all ESP32-H2 GPIOs that could
  be used as ADC channels.
- GPIO6 and GPIO7 are not broken out for any reason.
- GPPIO15 to GPIO21 are used for the in-package flash and are not broken out.

For detailed information about the peripheral configurations of ESP32-H2
boards, see section \ref esp32_peripherals "Common Peripherals".

[Back to table of contents](#esp32h2_devkit_toc)

### Board Pinout {#esp32h2_devkit_pinout}

The following figures show the pinout as configured by default board
definition.

@image html https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32h2/_images/esp32-h2-devkitm-1-v1.2_pinlayout.png "EPS32-H2-DevKitM-1 Pinout"

The corresponding board schematics can be found:

- [ESP32-H2-DevKitM-1 v1.2](https://dl.espressif.com/dl/schematics/esp32-h2-devkitm-1_v1.2_schematics.pdf)
- [ESP32-H2-DevKitM-1 v1.3](https://dl.espressif.com/dl/schematics/esp32-h2-devkitm-1_v1.3_schematics.pdf)

[Back to table of contents](#esp32h2_devkit_toc)

## Flashing the Device {#esp32h2_devkit_flashing}

Flashing RIOT is quite easy. The board has a Micro-USB connector with
reset/boot/flash logic. Just connect the board to your host computer
and type using the programming port:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make flash BOARD=esp32h2-devkit ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For detailed information about ESP32-H2 as well as configuring and compiling
RIOT for ESP32-H2 boards, see \ref esp32_riot.

[Back to table of contents](#esp32h2_devkit_toc)
