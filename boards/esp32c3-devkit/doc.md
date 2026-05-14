<!--
SPDX-FileCopyrightText: 2022 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards_esp32c3_devkit ESP32-C3-DevKit Board
@ingroup    boards_esp32c3
@brief      Support for generic ESP32-C3 boards
@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32c3_devkit ESP32-C3-DevKit

## Overview

The Espressif ESP32-C3-DevKit boards are a couple of boards that use one of
the following modules:

- ESP32-C3-MINI-1x module (ESP32-C3-DevKitM-1 board)
- ESP32-C3-WROOM-02x module (ESP32-C3-DevKitC-02 board)

Since the number of GPIOs and their possible uses on the ESP32-C3 are quite
limited, the ESP32-C3-DevKit should also work for most other ESP32-C3 boards.
Any modifications required for specific applications could be overridden by
\ref esp32_application_specific_configurations "application-specific board configuration".

<img src="https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c3/_images/esp32-c3-devkitm-1-v1-annotated-photo.png" alt="Espressif ESP32-C3-DevKitM-1" width=800px />

## Hardware

### MCU

Most features of the board are provided by the ESP32-C3 SoC. For detailed
information about the ESP32-C3 variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

### Board Configuration

ESP32-C3-DevKit boards have no special hardware on board with the exception
of a single pin RGB-LED.

All GPIOs are simply broken out for flexibility. Therefore, the board
configuration is the most flexible one which provides:

- 6 x ADC channels at maximum
- 1 x SPI
- 1 x I2C
- 1 x UART
- 2 x PWM with 2 channels each
- 1 RGB-LED

Since all GPIOs have broken out, GPIOs can be used for different purposes
in different applications. For flexibility, GPIOs can be listed in multiple
peripheral configurations, but they can only be used for one peripheral
at a time. For example, GPIO4 is used in the ADC channel definition, the
PWM channel definition and the definition of the SCL signal for I2C_DEV(0).

This is possible because GPIOs are only used for a specific peripheral
interface when either

- the corresponding peripheral module is used, e.g. `periph_i2c` and
  `periph_spi`, or
- the corresponding init function is called, e.g. `adc_init` and
  `pwm_init`.

That is, the purpose for which a GPIO is used depends on which module
or function is used first.

For example, if module `periph_i2c` is not used, the GPIOs listed in I2C
configuration can be used for the other purposes, that is, GPIO4 can be
used as ADC channel or PWM channel.

The following table shows the default board configuration, which is sorted
according to the defined functionality of GPIOs. This configuration can be
overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | GPIOs  | Remarks |Configuration
:---------------|:-------|:--------|:----------------------------------
BUTTON0         | GPIO9  |         | |
ADC             | GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5 | | \ref esp32_adc_channels "ADC Channels"
PWM_DEV(0)      | GPIO3, GPIO4 |   | \ref esp32_pwm_channels "PWM Channels"
PWM_DEV(1)      | GPIO10, GPIO7    | conflicts with SPI_DEV(0) | \ref esp32_pwm_channels "PWM Channels"
I2C_DEV(0):SCL  | GPIO4  |         | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0):SDA  | GPIO5  |         | \ref esp32_i2c_interfaces "I2C Interfaces"
RGB-LED         | GPIO8  | supported by driver module `ws281x` | |
SPI_DEV(0):CLK  | GPIO6  | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MISO | GPIO2  | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MOSI | GPIO7  | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):CS0  | GPIO10 | SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0):TxD | GPIO21 | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0):RxD | GPIO20 | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
</center>
<br>
@note
- The configuration of ADC channels contains all ESP32-C3 GPIOs that could
  be used as ADC channels.
- PWM_DEV(1) is only available if SPI_DEV(0) is not used.

For detailed information about the peripheral configurations of ESP32-C3
boards, see section \ref esp32_peripherals "Common Peripherals".

### Board Pinout

The following figures show the pinouts as configured by default board
definition.

<img src="https://gitlab.com/gschorcht/RIOT.wiki-Images/-/raw/master/esp32/ESP32-C3-DevKitM-1_pinout_v2.png" alt="EPS32-C3-DevKitM-1x Pinout" />
<img src="https://gitlab.com/gschorcht/RIOT.wiki-Images/-/raw/master/esp32/ESP32-C3-DevKitC-02_pinout_v2.png" alt="EPS32-C3-DevKitC-02x Pinout" />

The corresponding board schematics can be found:

- [ESP32-C3-DevKitM-1](https://dl.espressif.com/dl/schematics/SCH_ESP32-C3-DEVKITM-1_V1_20200915A.pdf)
- [ESP32-C3-DevKitC-02](https://dl.espressif.com/dl/schematics/SCH_ESP32-C3-DEVKITC-02_V1_1_20210126A.pdf)

## Flashing the Device

Flashing RIOT is quite easy. The board has a Micro-USB connector with
reset/boot/flash logic. Just connect the board to your host computer
and type using the programming port:

```shell
BOARD=esp32c3-devkit make flash ...
```

For detailed information about ESP32-C3 as well as configuring and compiling
RIOT for ESP32-C3 boards, see \ref esp32_riot.
