<!--
SPDX-FileCopyrightText: 2023 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards_esp32s3_box ESP32-S3-Box
@ingroup    boards_esp32s3
@brief      Support for the ESP32-S3-Box
@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32s3_box ESP32-S3-Box

## Table of Contents {#esp32s3_box_toc}

1. [Overview](#esp32s3_box_overview)
2. [Hardware](#esp32s3_box_hardware)
    1. [MCU](#esp32s3_box_mcu)
    2. [Board Configuration](#esp32s3_box_board_configuration)
    3. [Board Pinout](#esp32s3_box_pinout)
3. [Flashing the Device](#esp32s3_box_flashing)

## Overview {#esp32s3_box_overview}

The Espressif ESP32-S3-Box is a AIoT development platform for the ESP32-S3 SoC.

\image html https://raw.githubusercontent.com/espressif/esp-box/master/docs/_static/esp32_s3_box.png "ESP32-S2-Box" width=400px
\image html https://obrazki.elektroda.pl/4333131800_1637933077_bigthumb.jpg "Hardware Overview" width=700px

The ESP32-S3-Box has following main features:

<center>
|Feature                                      | Support |
|:--------------------------------------------|:-------:|
| ESP32-S3 SoC                                | yes     |
| 16 MB Flash                                 | yes     |
| 8 MB Octal SPI RAM                          | yes     |
| 2.4\" LCD Display 320 x 240 with ILI9342C   | yes     |
| Capacitive Touch Panel                      | no      |
| Dual Microphone ES7210                      | no      |
| Speaker Codec ES8311                        | no      |
| 3-axis Gyroscope, 3-axis Accelerometer      | no      |
| 2 x Digilent Pmod(TM) Connectors            | yes     |
| USB Type-C                                  | yes     |
</center>
<br>

The Espressif ESP32-S3-Box is shipped with a Dock which exposes the
2 x Digilent Pmod(TM) Connectors.

[Back to table of contents](#esp32s3_box_toc)

## Hardware {#esp32s3_box_hardware}

This section describes

- the [MCU](#esp32s3_box_mcu),
- the default [board configuration](#esp32s3_box_board_configuration),
- the [board pinout](#esp32s3_box_pinout).

[Back to table of contents](#esp32s3_box_toc)

### MCU {#esp32s3_box_mcu}

Most features of the ESP32-S3-Box are provided by the ESP32-S3 SoC. For detailed
information about the ESP32-S3 SoC variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

[Back to table of contents](#esp32s3_box_toc)

### Board Configuration {#esp32s3_box_board_configuration}

The following table shows the default ESP32-S3-Box configuration, which is
sorted according to the defined functionality of GPIOs. This configuration can
be overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | GPIOs  | Remarks | Configuration
:---------------|:-------|:--------|:----------------------------------
BUTTON0         | GPIO0  | Boot Mode Button | |
I2C_DEV(0) SCL  | GPIO40 | PMOD1 | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0) SDA  | GPIO41 | PMOD1 | \ref esp32_i2c_interfaces "I2C Interfaces"
SPI_DEV(0) CLK  | GPIO12 | PMOD2, SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MISO | GPIO13 | PMOD2, SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MOSI | GPIO11 | PMOD2, SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) CS0  | GPIO10 | PMOD2, SPI2_HOST (FSPI) is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) CLK  | GPIO7  | LCD SCK, SPI3_HOST is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) MISO | GPIO35 | not used, SPI3_HOST is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) MOSI | GPIO6  | LCD SDA, SPI3_HOST is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) CS0  | GPIO5  | LCD CS, SPI3_HOST is used | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0) TxD | GPIO43 | PMOD2 | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0) RxD | GPIO44 | PMOD2 | \ref esp32_uart_interfaces "UART interfaces"
</center>
\n

[Back to table of contents](#esp32s3_box_toc)

### Board Pinout {#esp32s3_box_pinout}

The following figures show the pinouts as configured by default board
definition.

@image html https://raw.githubusercontent.com/espressif/esp-box/master/docs/_static/previous_get_started_fig/hardware_pmod.png "ESP32-S3-BoxC-1 Pinout" width=900px

The corresponding schematics can be found:

- [ESP32-S3-Box](https://github.com/espressif/esp-box/blob/master/hardware/esp32_s3_box_v2.5/schematic/SCH_ESP32-S3-BOX_V2.5_20211011.pdf)
- [ESP32-S3-Box Dock](https://github.com/espressif/esp-box/blob/master/hardware/esp32_s3_box_dock_v1.0/schematic/ESP32-S3-BOX-DOCK_V1D0_20210922.pdf)

[Back to table of contents](#esp32s3_box_toc)

## Flashing the Device {#esp32s3_box_flashing}

Since the ESP32-S3-Box does not have a USB-to-Serial chip, the easiest way to
flash it is using the USB Serial/JTAG interface. Just connect the ESP32-S3-Box
to your host computer and use the following command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BOARD=esp32s3-box make flash ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Usually the make system resets the ESP32-S3-Box before flashing to enable the
USB Serial/JTAG controller and to reboot the ESP32-S3 in download mode.

In some very special cases this reset does not work and the programmer cannot
connect to the card, so the flashing is aborted with a timeout:
```
Serial port /dev/ttyACM0
Connecting...
...
serial.serialutil.SerialTimeoutException: Write timeout
```
This can happen either if RIOT is not yet installed or if the USB port was
previously used with the USB OTG controller, for example with USBUS or tinyUSB.
In this case, restart the ESP32-S3-Box manually into download mode by pressing
and releasing the RESET button while holding down the BOOT button.
In download mode, the USB Serial/JTAG interface is always available.

@note If the USB port was previously used with the USB OTG controller, an
      automatic restart of the ESP32-S3 with the flashed application is not
      possible. In this case the ESP32-S3-Box must be reset manually using the RESET
      button. In all other cases the ESP32-S3 automatically restarts with
      the flashed application.

[Back to table of contents](#esp32s3_box_toc)
