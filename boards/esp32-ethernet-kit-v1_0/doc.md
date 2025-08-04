<!--
Copyright (C) 2018 Gunar Schorcht
Copyright (C) 2020 Google LLC

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
-->

@defgroup   boards_esp32_esp-ethernet-kit-v1_0 ESP32-Ethernet-Kit V1.0 Board
@ingroup    boards_esp32_esp-ethernet-kit
@brief      Support for for Espressif ESP32-Ethernet-Kit v1.0
@author     Gunar Schorcht <gunar@schorcht.net>
@author     Erik Ekman <eekman@google.com>

\section esp32-ethernet-kit-v1_0 Espressif ESP32-Ethernet-Kit V1.0

## Table of Contents {#esp32_ethernet_kit_v1_0_toc}

1. [Overview](#esp32_ethernet_kit_v1_0_overview)
2. [Hardware](#esp32_ethernet_kit_v1_0_hardware)
    1. [Board Configuration](#esp32_ethernet_kit_v1_0_board_configuration)
    2. [Board Pinout](#esp32_ethernet_kit_v1_0_pinout)
3. [Other Documentation Resources](#esp32_ethernet_kit_v1_0_other-resources)

## Overview {#esp32_ethernet_kit_v1_0_overview}

The Espressif [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-ethernet-kit/user_guide.html) is a development board that uses the ESP32-WROVER-B module. Most important features of the board are

- 100 Mbps Ethernet via IP101G PHY
- USB bridge with JTAG interface

Furthermore, some GPIOs are broken out for extension. The USB bridge based on FDI FT2232HL provides a JTAG interface for OCD debugging through the USB interface.
For flashing and debugging the board, see \ref boards_esp32_esp-ethernet-kit common board documentation.

@image html "https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/_images/esp32-ethernet-kit-v1.0.png" "ESP32-Ethernet-Kit V1.0" width=500

[Back to table of contents](#esp32_ethernet_kit_v1_0_toc)

## Hardware {#esp32_ethernet_kit_v1_0_hardware}

This section describes

- the default [board configuration](#esp32_ethernet_kit_v1_0_board_configuration),
- the [board pinout](#esp32_ethernet_kit_v1_0_pinout).

[Back to table of contents](#esp32_ethernet_kit_v1_0_toc)

### Board Configuration {#esp32_ethernet_kit_v1_0_board_configuration}

ESP32-Ethernet-Kit v1.0 has the following on-board components

- 100 Mbps Ethernet via IP101G PHY
- USB bridge with JTAG interface

For detailed information about the configuration of ESP32 boards, see
section Peripherals in \ref esp32_riot.

@note
Only a few GPIOs are broken out and available for external hardware on ESP32-Ethernet-Kit boards. Which GPIOs are available as peripherals depends on used modules.

<center>
Function        | GPIOs  | Remarks |Configuration
:---------------|:-------|:--------|:----------------------------------
BTN0            | GPIO0  | not available if `esp_eth` is used | |
ADC             | GPIO34, GPIO35, GPIO36, GPIO39 | | \ref esp32_adc_channels "ADC Channels"
DAC             | - | | \ref esp32_dac_channels "DAC Channels"
PWM_DEV(0)      | GPIO4 | | \ref esp32_pwm_channels "PWM Channels"
I2C_DEV(0):SDA  | GPIO32 | | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0):SCL  | GPIO33 | | \ref esp32_i2c_interfaces "I2C Interfaces"
SPI_DEV(0):CLK  | GPIO14 | HSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MISO | GPIO12 | HSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MOSI | GPIO13 | HSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):CS0  | GPIO15  | HSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0):TxD | GPIO1  | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0):RxD | GPIO3  | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
</center>
\n
@note
- GPIO4 only works properly on the board if the function switch
  for GPIO4 (DIP SW 5) is OFF.
- SPI_DEV(0) is not available if module `esp_jtag` is used. For the
  SPI_DEV(0) pins to work properly, the function switches (DIP switches)
  for the JTAG signals must be set to OFF.

[Back to table of contents](#esp32_ethernet_kit_v1_0_toc)

### Board Pinout {#esp32_ethernet_kit_v1_0_pinout}

The board schematic can be found [here](https://dl.espressif.com/dl/schematics/SCH_ESP32-ETHERNET-KIT_A_V1.0_20190517.pdf).

By default, only 3 bidirectional GPIO pins are unused: GPIO4, GPIO32, GPIO33.
The suggested configuration is for PWM and I2C, but they can also be used
for SPI or another serial port. By disabling the JTAG interface on the board,
another 4 GPIOs can be made available (GPIO12, GPIO13, GPIO14, GPIO15).

[Back to table of contents](#esp32_ethernet_kit_v1_0_toc)

## Other Documentation Resources {#esp32_ethernet_kit_v1_0_other-resources}

There is a comprehensive [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-ethernet-kit-v1.0.html) for the ESP32-Ethernet-Kit with a lot information about hardware configuration.

[Back to table of contents](#esp32_ethernet_kit_v1_0_toc)
