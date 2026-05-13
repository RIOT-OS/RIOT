<!--
SPDX-FileCopyrightText: 2023 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards_esp32s3_wt32_sc01_plus ESP32-S3 WT32-SC01 Plus
@ingroup    boards_esp32s3
@brief      Support for the ESP32-S3 WT32-SC01 Plus
@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32s3_wt32_sc01_plus ESP32-S3 WT32-SC01 Plus

## Overview

The [Wireless Tag WT32-SC01 Plus](http://en.wireless-tag.com/product-item-26.html)
is a smart panel development platform with the ESP32-S3 SoC.

<img src="https://raw.githubusercontent.com/sukesh-ak/ESP32-TUX/master/datasheet/WT32-SC01-Plus.png" alt="ESP32-S3 WT32-SC01 Plus" width=400px />

It also available on the market as
[Smart Panlee SC01 Plus](http://en.smartpanle.com/product-item-15.html).

The ESP32-S3 WT32-SC01 Plus has following main features:
<center>
|Feature                                      | Support |
|:--------------------------------------------|:-------:|
| ESP32-S3 SoC                                | yes     |
| 16 MB Flash                                 | yes     |
| 2 MB QSPI RAM                               | yes     |
| 3.5\" LCD Display 480 x 320 with ST7796UI   | yes     |
| Capacitive Touch Panel with FT6336U         | yes     |
| SD Card SPI mode                            | yes     |
| USB Type-C                                  | yes     |
| RS485 interface                             | no      |
</center>
<br>

## Hardware

### MCU

Most features of the ESP32-S3 WT32-SC01 Plus are provided by the ESP32-S3 SoC.
For detailed information about the ESP32-S3 SoC variant (family) and ESP32x
SoCs, see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

### Board Configuration

The following table shows the default ESP32-S3 WT32-SC01 Plus configuration,
which is sorted according to the defined functionality of the GPIOs.

<center>
Function        | GPIOs  | Remarks | Configuration
:---------------|:-------|:--------|:----------------------------------
I2C_DEV(0) SCL  | GPIO5  | Touch Panel | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0) SDA  | GPIO6  | Touch Panel | \ref esp32_i2c_interfaces "I2C Interfaces"
SPI_DEV(0) CLK  | GPIO39 | SD Card | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MISO | GPIO38 | SD Card | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MOSI | GPIO40 | SD Card | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) CS0  | GPIO41 | SD Card | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0) TxD | GPIO43 | DEBUG interface | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0) RxD | GPIO44 | DEBUG interface | \ref esp32_uart_interfaces "UART interfaces"
LCD BACKLIGHT   | GPIO45 | LCD Backlight | |
LCD RESET       | GPIO4  | LCD reset | |
LCD RS          | GPIO0  | LCD Command/Data Selection | |
LCD WR          | GPIO47 | LCD Write Clock | |
LCD TE          | GPIO48 | LCD Frame Sync | |
LCD DB0         | GPIO9  | LCD 8-bit parallel interface D0 | |
LCD DB1         | GPIO46 | LCD 8-bit parallel interface D1 | |
LCD DB2         | GPIO3  | LCD 8-bit parallel interface D2 | |
LCD DB3         | GPIO8  | LCD 8-bit parallel interface D3 | |
LCD DB4         | GPIO18 | LCD 8-bit parallel interface D4 | |
LCD DB5         | GPIO17 | LCD 8-bit parallel interface D5 | |
LCD DB6         | GPIO16 | LCD 8-bit parallel interface D6 | |
LCD DB7         | GPIO15 | LCD 8-bit parallel interface D7 | |
TP INT          | GPIO7  | Touch Interrupt | |
TP RST          | GPIO4  | Touch Reset | |
</center>
<br>

### Board Pinout

The pinout including the schematics of the board are described in detail in the
[data sheet](https://www.antratek.de/media/wysiwyg/pdf/WT32-SC01-Plus-V1.3-EN.pdf).

The board has different external interfaces that can be used to connect
external hardware:

<b>Extended I/O Interface (EXT)</b>

Pin | Description
----|:-----------
1   | 5V
2   | GND
3   | EXT_IO1 (GPIO10)
4   | EXT_IO2 (GPIO11)
5   | EXT_IO3 (GPIO12)
6   | EXT_IO4 (GPIO13)
7   | EXT_IO5 (GPIO14)
8   | EXT_IO6 (GPIO21)

<br>

<b>RS485 Interface</b>

Pin | Description
----|:-----------
1   | RS485-A
2   | RS485-B
3   | GND
4   | 5V

<br>

<b>Debug Interface (DEBUG)</b>

Pin | Description
----|:-----------
1   | 5V
2   | 3V3
3   | UART_DEV(0) TxD
4   | UART_DEV(0) RxD
5   | RESET (EN)
6   | BOOT (GPIO0)
7   | GND

<br>

## Flashing the Device

Since the ESP32-S3 WT32-SC01 Plus does not have a USB-to-Serial chip, the
easiest way to flash it is using the USB Serial/JTAG interface. Just connect
the ESP32-S3 WT32-SC01 Plus to your host computer and use the following command:

```shell
BOARD=esp32s3-wt32-sc01-plus make flash ...
```

Usually the make system resets the ESP32-S3 WT32-SC01 Plus before flashing to
enable the USB Serial/JTAG controller and to reboot the ESP32-S3 in download
mode.

## Debugging with the Device

To be able to debug with the board, it must not use the STDIO via the USB OTG
interface as it does by default. Instead, the STDIO must use the UART interface.
For this purpose, compile and flash the application with the following command:

```shell
USEMODULE=stdio_uart BOARD=esp32s3-wt32-sc01-plus make flash ...
```

In order to use the STDIO in a terminal in this case, a debugging tool is
available on the market, which is connected to the debug interface of the board.

@note The debugging tool does not mean that it is used as OpenOCD adapter. It
      simply exposes the UART interface.

For detailed information on debugging an ESP32-S3 board, refer to the section
[JTAG Debugging with ESP32x SoC](#esp32_jtag_debugging).
