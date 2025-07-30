<!--
Copyright (C) 2022 Gunar Schorcht

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
-->

@defgroup   boards_esp32s2_lilygo_ttgo_t8 LILYGO TTGO T8 ESP32-S2 Board
@ingroup    boards_esp32s2
@brief      Support for LILYGO TTGO T8 ESP32-S2 Board
@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32s2_lilygo_ttgo_t8 LILYGO TTGO T8 ESP32-S2

## Table of Contents {#esp32s2_lilygo_ttgo_t8_toc}

1. [Overview](#esp32s2_lilygo_ttgo_t8_overview)
2. [Hardware](#esp32s2_lilygo_ttgo_t8_hardware)
    1. [MCU](#esp32s2_lilygo_ttgo_t8_mcu)
    2. [Board Configuration](#esp32s2_lilygo_ttgo_t8_board_configuration)
    3. [Board Pinout](#esp32s2_lilygo_ttgo_t8_pinout)
3. [Flashing the Device](#esp32s2_lilygo_ttgo_t8_flashing)

## Overview {#esp32s2_lilygo_ttgo_t8_overview}

The Espressif LILYGO TTGO T8 ESP32-S2 (also known as LilyGo T-Display S2) is
a ESP32-S2 board with an OLED Display and a TF Card slot.

\image html https://ae01.alicdn.com/kf/H8062f551fe7b4233809294d4a5b1d45d1.jpg "LILYGO TTGO T8 ESP32-S2" width=600px

The board has following main features:

Feature              | Support
:--------------------|:-------
ESP32-S2 SoC         | yes
4 MB Flash           | yes
8 MB QSPI RAM        | yes
SD Card slot         | yes
32.768KHz Crystal    | yes
OLED display ST77789 | yes

[Back to table of contents](#esp32s2_lilygo_ttgo_t8_toc)

## Hardware {#esp32s2_lilygo_ttgo_t8_hardware}

This section describes

- the [MCU](#esp32s2_lilygo_ttgo_t8_mcu),
- the default [board configuration](#esp32s2_lilygo_ttgo_t8_board_configuration),
- the [board pinout](#esp32s2_lilygo_ttgo_t8_pinout).

[Back to table of contents](#esp32s2_lilygo_ttgo_t8_toc)

### MCU {#esp32s2_lilygo_ttgo_t8_mcu}

Most features of the board are provided by the ESP32-S2 SoC. For detailed
information about the ESP32-S2 SoC variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

[Back to table of contents](#esp32s2_lilygo_ttgo_t8_toc)

### Board Configuration {#esp32s2_lilygo_ttgo_t8_board_configuration}

The LILYGO TTGO T8 ESP32-S2 board uses the `SPI_DEV(0)` (FSPI) for the
OLED Display. The GPIOs of `SPI_DEV(0)` are therefore not broken out.
SPI_DEV(1) is used for the SD Card slot. These GPIOs are broken out and
can also be used by other devices.

The LILYGO TTGO T8 ESP32-S2 has a USB-to-UART bridge on board. The USB D-/D+
signals of the USB-C connector are connected via DIP switches either to this
USB-to-UART bridge (default) or to GPIO19/GPIO20 of the ESP32-S2. To use the
USB OTG interface of the ESP32-S2, the USB D-/D+ signals must be connected to
GPIO19 and GPIO20, see section \ref esp32s2_lilygo_ttgo_t8_pinout "Pinout".
To use the USB OTG interface also for the console in this case, either the
`stdio_cdc_acm` or the `stdio_tinyusb_cdc_acm` module must be enabled.
This is automatically the case if the USBUS or tinyUSB stack is used.

The LILYGO TTGO T8 ESP32-S2 has a 32.768 kHz crystal on the board, which is
connected to GPIO15 and GPIO16 via DIP switches (default). The crystal can
be switched off via the DIP switches so that GPIO15 and GPIO16 become available
at the headers, see section \ref esp32s2_lilygo_ttgo_t8_pinout "Pinout".

The following table shows the default board configuration, which is sorted
according to the defined functionality of GPIOs. This configuration can be
overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | GPIOs  | Remarks | Configuration
:---------------|:-------|:--------|:----------------------------------
BUTTON0         | GPIO0  | | |
ADC_LINE(n)     | GPIO1 ... GPIO9 | | \ref esp32_adc_channels "ADC Channels"
DAC_LINE(n)     | GPIO17, GPIO18 | | \ref esp32_dac_channels "DAC Channels"
GPIO38          | GPIO38 | OLED RESET (not broken out) | |
I2C_DEV(0) SCL  | GPIO7 | | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0) SDA  | GPIO8 | | \ref esp32_i2c_interfaces "I2C Interfaces"
PWM_DEV(0)      | GPIO39, GPIO40, GPIO41, GPIO42 | - | \ref esp32_pwm_channels "PWM Channels"
PWM_DEV(1)      | GPIO15, GPIO16 | if module `esp_rtc_timer_32k` is not used | \ref esp32_pwm_channels "PWM Channels"
SPI_DEV(0) CLK  | GPIO36 | OLED CLK (not broken out) | |
SPI_DEV(0) MISO | GPIO37 | OLED MISO (not broken out) | |
SPI_DEV(0) MOSI | GPIO35 | OLED MOSI (not broken out) | |
SPI_DEV(0) CS0  | GPIO34 | OLED CS0 (not broken out) | |
SPI_DEV(1) CLK  | GPIO12 | SD Card CLK | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) MISO | GPIO37 | SD Card MISO | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) MOSI | GPIO35 | SD Card MOSI | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) CS0  | GPIO34 | SD Card CS | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0) TxD | GPIO43 | Console | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0) RxD | GPIO44 | Console | \ref esp32_uart_interfaces "UART interfaces"
USB D-          | GPIO19 | if USB-to-UART bridge is not used | |
USB D+          | GPIO20 | if USB-to-UART bridge is not used | |
</center>
\n

For detailed information about the peripheral configurations of ESP32-S2
boards, see section \ref esp32_peripherals "Common Peripherals".

[Back to table of contents](#esp32s2_lilygo_ttgo_t8_toc)

### Board Pinout {#esp32s2_lilygo_ttgo_t8_pinout}

The following figure shows the pinout as configured by the board
definition.

@image html https://ae01.alicdn.com/kf/H4a77f8684c144384a165d7a89476c602q.jpg "LILYGO TTGO T8 ESP32-S2 Pinout" width=900px

The corresponding board schematics can be found [here](https://github.com/Xinyuan-LilyGO/LilyGo-T-Display-S2/raw/master/schematic/ESP32_S2-Display.pdf)

[Back to table of contents](#esp32s2_lilygo_ttgo_t8_toc)

## Flashing the Device {#esp32s2_lilygo_ttgo_t8_flashing}

Flashing RIOT is quite easy. The board has an USB-C connector which is either
connected to the USB-to-UART bridge (default) with a built-in reset/boot/flash
logic or to the USB signals of the ESP32-S2 SoC. The flash method depends on
the configuration of the DIP switches,
see section \ref esp32s2_lilygo_ttgo_t8_pinout "Pinout".

### USB-C connector is connected to the USB-to-UART bridge (default)

Just connect the board to your host computer and type using the programming
port:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BOARD=esp32s2-liligo-ttgo-t8 make flash ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For detailed information about ESP32-S2 as well as configuring and compiling
RIOT for ESP32-S2 boards, see \ref esp32_riot.

### USB-C connector is connected to the USB signals of the ESP32-S2 SoC

In this configuration the integrated USB-to-JTAG bridge can be used to flash
the board. The USB-to-JTAG bridge is automatically activated in the download
mode of the bootloader, provided that the eFuses `JTAG_SEL_ENABLE` and
`DIS_USB_JTAG` are not burned (see also section
\ref esp32_jtag_interface_esp32s2 "JTAG Interface" strapping pin GPIO3).

For this purpose, connect the board to your host computer and enter the
following command using the programming port:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE=esp32s2-lilygo-ttgo-t8-usb BOARD=esp32s2-liligo-ttgo-t8 make flash ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If RIOT is already on the device, it should automatically reset the device
and enter the bootloader download mode. If

- RIOT is not already on the device or
- automatic reset does not work or
- the device is stuck in some other mode,

the download mode has to be forced before by pressing the Reset (RST) button
while holding the BOOT (BOT) button.

[Back to table of contents](#esp32s2_lilygo_ttgo_t8_toc)
