<!--
Copyright (C) 2025 David Picard

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
-->

@defgroup    boards_seeedstudio-xiao-esp32c3 Seeed Studio Xiao ESP32C3
@ingroup     boards_esp32c3
@brief       Support for the Seeed Studio Xiao ESP32-C3 board
@author      David Picard

\section seeedstudio-xiao-esp32c3 Seeed Studio Xiao ESP32C3

## Table of Contents {#seeedstudio-xiao-esp32c3_toc}

1. [Overview](#seeedstudio-xiao-esp32c3_overview)
2. [Hardware](#seeedstudio-xiao-esp32c3_hardware)
    1. [MCU](#seeedstudio-xiao-esp32c3_mcu)
    2. [Board Configuration](#seeedstudio-xiao-esp32c3_board_configuration)
    3. [Board Pinout](#seeedstudio-xiao-esp32c3_pinout)
    4. [Board documentation](#seeedstudio-xiao-esp32c3_documentation)
3. [Flashing the Device](#seeedstudio-xiao-esp32c3_flashing)

## Overview {#seeedstudio-xiao-esp32c3_overview}

Seeed Studio Xiao ESP32C3 is an IoT mini development board based on the Espressif ESP32-C3
WiFi/Bluetooth dual-mode chip, featuring a single ­core 32-bit RISC-V CPU that operates at up to 160 MHz.

- Wi-Fi
- Bluetooth
- Battery charging chip: Supports lithium battery charge and discharge management
- Memory: 400KB of SRAM, and 4MB of on-board flash memory
- Small size: 21 x 17.8mm
- Ultra-Low Power: Deep sleep power consumption is about 43μA
- Battery charge indicator red LED

@image html https://files.seeedstudio.com/wiki/XIAO_WiFi/board-pic.png "Seeed Studio EPS32-C3 Xiao" width=200pix

[Back to table of contents](#seeedstudio-xiao-esp32c3_toc)

## Hardware {#seeedstudio-xiao-esp32c3_hardware}

This section describes

- the [MCU](#seeedstudio-xiao-esp32c3_mcu),
- the default [board configuration](#seeedstudio-xiao-esp32c3_board_configuration),
- the [board pinout](#seeedstudio-xiao-esp32c3_pinout).

[Back to table of contents](#seeedstudio-xiao-esp32c3_toc)

### MCU {#seeedstudio-xiao-esp32c3_mcu}

Most features of the board are provided by the ESP32-C3 SoC. For detailed
information about the ESP32-C3 variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

[Back to table of contents](#seeedstudio-xiao-esp32c3_toc)

### Board Configuration {#seeedstudio-xiao-esp32c3_board_configuration}

The Seeed Studio EPS32-C3 Xiao board has a reset button and a bootloader button,
but no LED.
After reset, the bootloader button may be used by the application.

To select the board, add the following to the make command line:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
BOARD=seeedstudio-xiao-esp32c3 make ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

GPIO overview:
- 3 x ADC channels at maximum
- 1 x SPI
- 1 x I2C
- 1 x UART
- 11 x PWM channels (only 4 defined by default)

The purpose for which a GPIO is used depends on which module
or function is used first. For example, if module `periph_spi` is not used,
the GPIOs listed in SPI configuration can be used for other purposes.

The following table shows the default board configuration.
This configuration can be overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | Pin                   | Configuration
:---------------|:----------------------|:-------------
BUTTON0         | GPIO9                 | GPIO9 is a [strapping pin](https://docs.espressif.com/projects/esptool/en/latest/esp32c3/advanced-topics/boot-mode-selection.html)
ADC             | GPIO2, GPIO3, GPIO4   | \ref esp32_adc_channels "ADC Channels"
I2C_DEV(0):SCL  | GPIO7                 | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0):SDA  | GPIO6                 | \ref esp32_i2c_interfaces "I2C Interfaces"
PWM_DEV(0)      | GPIO2, GPIO3, GPIO4, GPIO5 | \ref esp32_pwm_channels "PWM Channels"
SPI_DEV(0):SCK  | GPIO8                 | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MISO | GPIO9                 | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MOSI | GPIO10                | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):CS0  | GPIO5                 | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0):TxD | GPIO21                | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0):RxD | GPIO20                | \ref esp32_uart_interfaces "UART interfaces"
</center>
\n
@note The configuration of ADC channels contains all ESP32-C3 GPIOs that could
      be used as ADC channels.

For detailed information about the peripheral configurations of ESP32-C3
boards, see section \ref esp32_peripherals "Common Peripherals".

[Back to table of contents](#seeedstudio-xiao-esp32c3_toc)

### Board Pinout {#seeedstudio-xiao-esp32c3_pinout}

The following figures show the pinouts as configured by default board
definition.

@image html https://files.seeedstudio.com/wiki/XIAO_WiFi/pin_map-2.png "Seeed Studio EPS32-C3 Xiao Pinout" width=800

[Back to table of contents](#seeedstudio-xiao-esp32c3_toc)

### Board documentation {#seeedstudio-xiao-esp32c3_documentation}

- [schematic](https://files.seeedstudio.com/wiki/XIAO_WiFi/Resources/Seeeduino-XIAO-ESP32C3-SCH.pdf) (PDF)
- [pinout](https://files.seeedstudio.com/wiki/XIAO_WiFi/Resources/XIAO-ESP32C3-pinout_sheet.xlsx) (XLSX)
- [ESP32-C3 datasheet](https://files.seeedstudio.com/wiki/Seeed-Studio-XIAO-ESP32/esp32-c3_datasheet.pdf) (PDF)
- [ESP32-C3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf) (PDF)
- [power consumption report](https://files.seeedstudio.com/wiki/Seeed-Studio-XIAO-ESP32/Low_Power_Consumption.pdf) (PDF)
- [product page](https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5431.html?qid=eyJjX3NlYXJjaF9xdWVyeSI6InhpYW8gZXNwMzJjMyIsImNfc2VhcmNoX3Jlc3VsdF9wb3MiOjQsImNfdG90YWxfcmVzdWx0cyI6ODcsImNfc2VhcmNoX3Jlc3VsdF90eXBlIjoiUHJvZHVjdCIsImNfc2VhcmNoX2ZpbHRlcnMiOiJzdG9yZUNvZGU6W3JldGFpbGVyXSJ9)
- [wiki page](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/)

[Back to table of contents](#seeedstudio-xiao-esp32c3_toc)

## Flashing the Device {#seeedstudio-xiao-esp32c3_flashing}

The USB-C connector of the board is directly connected to the USB Serial/JTAG
interface of the ESP32-C3 SoC. It can be used to program the board and to debug
the application. Just connect the board to your host computer and use the
following command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make flash BOARD=seeedstudio-xiao-esp32c3 ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The make utility will normally restart the board in download mode in order
to flash it.
But, on occasion, this reset doesn't work.
The programmer can't connect to the board and the flashing operation is
aborted with a timeout error:
```
Serial port /dev/ttyACM0
Connecting...
...
serial.serialutil.SerialTimeoutException: Write timeout
```
In this situation, restart the board manually in download mode:
1. hold down the BOOT button
2. press and release the RESET button
3. release the BOOT button.

After flashing the board, it will still be in download mode. You have to press
the RESET button to start your application.

For detailed information about ESP32-C3 as well as configuring and compiling
RIOT for ESP32-C3 boards, see \ref esp32_riot.

[Back to table of contents](#seeedstudio-xiao-esp32c3_toc)
