<!--
Copyright (C) 2022 Gunar Schorcht

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
-->

@defgroup   boards_esp32s3_devkit ESP32-S3-DevKit Board
@ingroup    boards_esp32s3
@brief      Support for generic ESP32S3 boards
@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32s3_devkit ESP32-S3-DevKit

## Table of Contents {#esp32s3_devkit_toc}

1. [Overview](#esp32s3_devkit_overview)
2. [Hardware](#esp32s3_devkit_hardware)
    1. [MCU](#esp32s3_devkit_mcu)
    2. [Board Configuration](#esp32s3_devkit_board_configuration)
    3. [Board Pinout](#esp32s3_devkit_pinout)
3. [Flashing the Device](#esp32s3_devkit_flashing)

## Overview {#esp32s3_devkit_overview}

The Espressif ESP32-S3-DevKit boards are a couple of boards that use one of
the following modules:

- ESP32-S3-MINI-1x module (ESP32-S3-DevKitM-1 board)
- ESP32-S3-WROOM-1x module (ESP32-S3-DevKitC-1 board)
- ESP32-S3-WROOM-2 module (ESP32-S3-DevKitC-1 board)

\image html "https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/_images/ESP32-S3-DevKitC-1_v2-annotated-photo.png" "Espressif ESP32-S3-DevKitM-1" width=600px

<br>
Due to the different modules used, the available versions of the
ESP32-S3-DevKit board differ regarding the Flash size, the integrated SPI RAM
and the SPI voltage. To be able to use all these different versions of the
board with a single board definition, used board version can be specified
by the variable `BOARD_VERSION` during compilation, for example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BOARD=esp32s3-devkit BOARD_VERSION=esp32s3-devkitc-1-n8 make ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following table shows the available board versions, the size of the
Flash and the SPI RAM as well as the value of the variable `BOARD_VERSION`
that is used to specify the board version.

<center>
| Board Version             | Flash (Mode)  | SPI RAM (Mode) | `BOARD_VERSION`
|:--------------------------|:--------------|:---------------|:-------------------
| <b>ESP32-S3-DevKitC-1-N8  | 8 MB (Quad)   | -              | `esp32s3-devkitc-1-n8` (default)</b>
| ESP32-S3-DevKitC-1-N8R2   | 8 MB (Quad)   | 2 MB (Quad)    | `esp32s3-devkitc-1-n8r2`
| ESP32-S3-DevKitC-1-N8R8   | 8 MB (Quad)   | 8 MB (Octal)   | `esp32s3-devkitc-1-n8r8`
| ESP32-S3-DevKitC-1-N16R8V | 16 MB (Octal) | 8 MB (Octal)   | `esp32s3-devkitc-1-n16r8v`
| ESP32-S3-DevKitC-1-N32R8V | 32 MB (Octal) | 8 MB (Octal)   | `esp32s3-devkitc-1-n32r8v`
| ESP32-S3-DevKitC-1U-N8    | 8 MB (Quad)   | -              | `esp32s3-devkitc-1u-n8`
| ESP32-S3-DevKitC-1U-N8R2  | 8 MB (Quad)   | 2 MB (Quad)    | `esp32s3-devkitc-1u-n8r2`
| ESP32-S3-DevKitC-1U-N8R8  | 8 MB (Quad)   | 8 MB (Octal)   | `esp32s3-devkitc-1u-n8r8`
| ESP32-S3-DevKitM-1-N8     | 8 MB (Quad)   | -              | `esp32s3-devkitm-1-n8`
| ESP32-S3-DevKitM-1U-N8    | 8 MB (Quad)   | -              | `esp32s3-devkitm-1u-n8`
</center>
<br>

@note
- If the board version is not specified, <b>ESP32-S3-DevKitC-1-N8 with 8 MByte
  Flash</b> is assumed and `BOARD_VERSION` is set to <b>`esp32s3-devkitc-1-n8`</b>
  by default.
- Using a board version with embedded SPI RAM (`BOARD_VERSION` is any of
  <b>`esp32s3-devkitx-1x-*r*`</b> values) enables the
  \ref esp32_spi_ram "esp_spi_ram" feature. The SPI RAM can then be used as
  heap by enabling the \ref esp32_spi_ram "esp_spi_ram" pseudomodule.
- Depending on the specified board version, one of the following modes is used
  for the SPI RAM:
  - **Quad SPI mode**: If `BOARD_VERSION` is one of the values
    <b>`esp32s3_devkitx-1x-*r2`</b>, the Quad SPI mode is used. In this mode,
    **GPIO26 ... GPIO32** are occupied and are not available for other purposes.
  - **Octal SPI mode**: If `BOARD_VERSION` is one of the values
    <b>`esp32s3_devkitx-1x-*r8x`</b>, the Octal-SPI mode is used and the
    feature \ref esp32_spi_ram "esp_spi_oct" is additionally enabled. If
    then the SPI RAM is activated by using the pseudomodule \ref esp32_spi_ram
    "esp_spi_ram", **GPIO33 ... GPIO37** are occupied and are not available
    for other purposes. Conflicts may then occur when using these GPIOs.

[Back to table of contents](#esp32s3_devkit_toc)

## Hardware {#esp32s3_devkit_hardware}

This section describes

- the [MCU](#esp32s3_devkit_mcu),
- the default [board configuration](#esp32s3_devkit_board_configuration),
- [optional hardware configurations](#esp32s3_devkit_optional_hardware),
- the [board pinout](#esp32s3_devkit_pinout).

[Back to table of contents](#esp32s3_devkit_toc)

### MCU {#esp32s3_devkit_mcu}

Most features of the board are provided by the ESP32-S3 SoC. For detailed
information about the ESP32-S3 SoC variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

[Back to table of contents](#esp32s3_devkit_toc)

### Board Configuration {#esp32s3_devkit_board_configuration}

ESP32-S3-DevKit boards have no special hardware on board with the exception
of a single pin RGB-LED.

All GPIOs are simply broken out for flexibility. Therefore, the board
configuration is the most flexible one which provides:

- 20 x ADC channels at maximum
- 2 x SPI
- 1 x I2C
- 2 x UART
- 1 RGB-LED

Since almost GPIOs have broken out, GPIOs can be used for different purposes
in different applications. Following GPIOs are not broken out:

- ESP32-S3-DevKitC-1x: GPIO26..GPIO34
- ESP32-S3-DevKitM-1x: GPIO27..GPIO32

For flexibility, GPIOs can be used in multiple peripheral configurations,
but they can only be used for one peripheral at a time. For example, GPIO9
is used in the ADC channel definition and the definition of the SCL signal
for I2C_DEV(0).

This is possible because GPIOs are only used for a specific peripheral
interface when either

- the corresponding peripheral module is used, e.g. `periph_i2c`, or
- the corresponding init function is called, e.g. `adc_init`.

That is, the purpose for which a GPIO is used depends on which module
or function is used first.

For example, if module `periph_i2c` is not used, the GPIOs listed in I2C
configuration can be used for the other purposes, that is, GPIO9 can be
used as ADC channel.

The following table shows the default board configuration, which is sorted
according to the defined functionality of GPIOs. This configuration can be
overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | GPIOs  | Remarks | Configuration
:---------------|:-------|:--------|:----------------------------------
BUTTON0         | GPIO0  | | |
ADC_LINE(n)     | GPIO1, GPIO2, GPIO4, GPIO5, GPIO6, GPIO7, GPIO8, GPIO9 | | \ref esp32_adc_channels "ADC Channels"
PWM_DEV(0)      | GPIO14, GPIO17, GPIO18, GPIO21 | - | \ref esp32_pwm_channels "PWM Channels"
PWM_DEV(1)      | GPIO15, GPIO16 | if module `esp_rtc_timer_32k` is not used | \ref esp32_pwm_channels "PWM Channels"
I2C_DEV(0) SCL  | GPIO9  | | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0) SDA  | GPIO8  | | \ref esp32_i2c_interfaces "I2C Interfaces"
RGB-LED         | GPIO48 | supported by driver module `ws281x` | |
SPI_DEV(0) CLK  | GPIO12 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MISO | GPIO13 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MOSI | GPIO11 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) CS0  | GPIO10 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0) TxD | GPIO43 | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0) RxD | GPIO44 | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
</center>
\n

@note The RGB-LED is connected to GPIO38 on ESP32-S3-DevKitC-1 revision 1.1 boards.

For detailed information about the peripheral configurations of ESP32-S3
boards, see section \ref esp32_peripherals "Common Peripherals".

[Back to table of contents](#esp32s3_devkit_toc)

### Board Pinout {#esp32s3_devkit_pinout}

The following figures show the pinouts as configured by default board
definition.

@image html "https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/_images/ESP32-S3_DevKitC-1_pinlayout_v1.1.jpg" "ESP32-S3-DevKitC-1 Pinout" width=900px
@image html "https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/_images/ESP32-S3_DevKitM-1_pinlayout.jpg" "ESP32-S3-DevKitM-1 Pinout" width=900px

The corresponding board schematics can be found:

- [ESP32-S3-DevKitC-1](https://dl.espressif.com/dl/SCH_ESP32-S3-DEVKITC-1_V1_20210312C.pdf)
- [ESP32-S3-DevKitM-1](https://dl.espressif.com/dl/schematics/SCH_ESP32-S3-DEVKITM-1_V1_20210310A.pdf)

[Back to table of contents](#esp32s3_devkit_toc)

## Flashing the Device {#esp32s3_devkit_flashing}

Flashing RIOT is quite easy. The board has a Micro-USB connector with
reset/boot/flash logic. Just connect the board to your host computer
and type using the programming port:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BOARD=esp32s3-devkit make flash ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For detailed information about ESP32-S3 as well as configuring and compiling
RIOT for ESP32-S3 boards, see \ref esp32_riot.

[Back to table of contents](#esp32s3_devkit_toc)
