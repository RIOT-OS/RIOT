<!--
SPDX-FileCopyrightText: 2023 Gunar Schorcht
SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards_waveshare-esp32s3-touch-lcd-128 Waveshare ESP32-S3-Touch-LCD-1.28
@ingroup    boards_esp32s3
@brief      Support for the Waveshare ESP32-S3-Touch-LCD-1.28 board
@author     Gunar Schorcht <gunar@schorcht.net>
@author     Yahia Abdella <yahia.abdella@tuhh.de>

\section waveshare_esp32s3_touch_lcd_128 Waveshare ESP32-S3-Touch-LCD-1.28

## Overview

The Waveshare ESP32-S3-Touch-LCD-1.28 is a compact development board built around the
ESP32-S3 microcontroller with an integrated 1.28-inch round TFT LCD display and
capacitive touch panel.

<img src="https://www.waveshare.net/photo/development-board/ESP32-S3-Touch-LCD-1.28/ESP32-S3-Touch-LCD-1.28-1.jpg" alt="Waveshare ESP32-S3-Touch-LCD-1.28" width="400" />

The main features of the board are:

- ESP32-S3-N8R2 SoC with 2.4 GHz WiFi (802.11 b/g/n) and Bluetooth 5, BLE
- 16 MB Flash
- 2 MB PSRAM
- 1.28-inch 240x240 round IPS LCD (GC9A01 driver)
- Capacitive touch panel (CST816S controller)
- 6-axis IMU (QMI8658 accelerometer + gyroscope)
- USB Type-C connector
- Battery charging circuit with battery voltage monitoring
- BOOT button (active low, directly usable as user button)

## Hardware

### MCU

Most features of the board are provided by the ESP32-S3 SoC. For detailed
information about the ESP32-S3 SoC variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

### Board Configuration

The board integrates a 1.28-inch round IPS LCD driven by a GC9A01 display controller
via SPI, a CST816S capacitive touch controller connected via I2C, and a QMI8658
6-axis IMU also connected via I2C. A BOOT button on GPIO0 is directly usable
as a user button.

The default board configuration provides:

- 1 x ADC channel (battery voltage monitoring)
- 1 x SPI (display)
- 1 x I2C (touch controller and IMU)
- 1 x UART (console)

The following table shows the default board configuration sorted by
peripheral function. This configuration can be overridden by
\ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | GPIOs  | Remarks | Configuration
:---------------|:-------|:--------|:----------------------------------
BTN0            | GPIO0  | | |
ADC_LINE(0)     | GPIO1  | Battery voltage | \ref esp32_adc_channels "ADC Channels"
SPI_DEV(0) CLK  | GPIO10 | LCD SCLK, GC9A01 | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MOSI | GPIO11 | LCD SDA, GC9A01 | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MISO | GPIO12 | LCD SDO, GC9A01 | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) CS0  | GPIO9  | LCD CS, GC9A01 | \ref esp32_spi_interfaces "SPI Interfaces"
LCD DC          | GPIO8  | GC9A01 data/command | |
LCD RST         | GPIO14 | GC9A01 reset | |
LCD BL          | GPIO2  | Backlight control | |
I2C_DEV(0) SCL  | GPIO7  | Touch + IMU | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0) SDA  | GPIO6  | Touch + IMU | \ref esp32_i2c_interfaces "I2C Interfaces"
Touch IRQ       | GPIO5  | CST816S interrupt | |
Touch RST       | GPIO13 | CST816S reset | |
UART_DEV(0) TxD | GPIO43 | Console (fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0) RxD | GPIO44 | Console (fixed) | \ref esp32_uart_interfaces "UART interfaces"
</center>
<br>

For detailed information about the peripheral configurations of ESP32-S3
boards, see section \ref esp32_peripherals "Common Peripherals".

### Board Pinout

The following figures show the board hardware layout.

<img src="https://docs.waveshare.com/assets/images/Esp32-s3-touch-lcd-1.28-003-c7bcf4bd1440b55a43abb060ad67d705.webp" alt="Waveshare ESP32-S3-Touch-LCD-1.28 Hardware Layout" width="600" />

<img src="https://docs.waveshare.com/assets/images/ESP32-S3-Touch-LCD-1.28-pin-e2103bcf84f511752bbc41ceb86bbf56.webp" alt="Waveshare ESP32-S3-Touch-LCD-1.28 Hardware Layout" width="600" />

The board schematic and additional documentation can be found on the
[Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.28).

## Flashing the Device

Flashing RIOT is quite easy. The board has a USB-C connector with
reset/boot/flash logic. Just connect the board to your host computer
and run:
For detailed information about ESP32-S3 as well as configuring and compiling
RIOT for ESP32-S3 boards, see \ref esp32_riot.
