<!--
SPDX-FileCopyrightText: 2023 Gunar Schorcht
SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards_elecrow-crowpanel-esp32s3-rotary-128 Elecrow CrowPanel ESP32-S3 Rotary LCD 1.28"
@ingroup    boards_esp32s3
@brief      Support for the Elecrow ESP32-S3 Rotary LCD 1.28" Board

## Overview

The Elecrow ESP32-S3 Rotary LCD 1.28" board is a compact development board
built around the ESP32-S3 microcontroller with an integrated 1.28"
round TFT LCD display, capacitive touch panel and a knurled rotary knob around
the display.

The main features of the board are:

- ESP32-S3R8 SoC with 2.4 GHz WiFi (802.11 b/g/n) and Bluetooth 5/BLE
- 16 MB Flash
- 2 MB PSRAM
- 1.28-inch 240x240 round IPS LCD (GC9A01 driver)
- Capacitive touch panel (CST816D controller)
- BOOT button (active low, directly usable as user button)
- Display Button
- Rotary knob around the display (currently unsupported, no `periph_qdec` for
  ESP32 implemented yet)
- Red LED, 5x WS281x compatible NeoPixels

## Hardware

### MCU

Most features of the board are provided by the ESP32-S3 SoC. For detailed
information about the ESP32-S3 SoC variant (family) and ESP32x SoCs,
see section @ref esp32_mcu_esp32 "ESP32 SoC Series".

### Board Configuration

The board integrates a 1.28-inch round IPS LCD driven by a GC9A01 display
controller via SPI and a CST816D capacitive touch controller connected via I2C.
A BOOT button on GPIO0 as well as the display button are directly usable
as user buttons.

The default board configuration provides:

- 2 x ADC channel (available on the external flat flex connector)
- 1 x SPI (display)
- 2 x I2C (touch controller and external flat flex connector)
- 1 x UART (console)
- 1 x LED Red via PWM Channel

The following table shows the default board configuration sorted by
peripheral function. This configuration can be overridden by
@ref esp32_application_specific_configurations
"application-specific configurations".

Function         | GPIOs  | Remarks                                         | Configuration
:----------------|:-------|:------------------------------------------------|:------------------------------------------
BTN0             | GPIO0  |                                                 |
BTN1             | GPIO41 | Display Button                                  |
ADC_LINE(0)      | GPIO4  | Flat Flex Connector                             | @ref esp32_adc_channels "ADC Channels"
ADC_LINE(1)      | GPIO12 | Flat Flex Connector                             | @ref esp32_adc_channels "ADC Channels"
RGB-LED          | GPIO48 | supported by the `ws281x` module                |
LED Red          | GPIO40 | Integrated Red LED                              |
SPI_DEV(0) CLK   | GPIO10 | LCD SCLK, GC9A01                                | @ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MOSI  | GPIO11 | LCD SDA, GC9A01                                 | @ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MISO  | UNDEF  | The LCD has no MISO output and can not be read. |
SPI_DEV(0) CS0   | GPIO9  | LCD CS, GC9A01                                  | @ref esp32_spi_interfaces "SPI Interfaces"
LCD DC           | GPIO3  | GC9A01 data/command                             |
LCD RST          | GPIO14 | GC9A01 reset                                    |
LCD BL           | GPIO46 | Backlight control (active high)                 |
I2C_DEV(0) SCL   | GPIO7  | Touch + IMU                                     | @ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0) SDA   | GPIO6  | Touch + IMU                                     | @ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(1) SCL   | GPIO39 | Flat Flex Connector                             | @ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(1) SDA   | GPIO38 | Flat Flex Connector                             | @ref esp32_i2c_interfaces "I2C Interfaces"
Touch IRQ        | GPIO5  | CST816D interrupt                               |
Touch RST        | GPIO13 | CST816D reset                                   |
UART_DEV(0) TxD  | GPIO43 | Console (fixed)                                 | @ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0) RxD  | GPIO44 | Console (fixed)                                 | @ref esp32_uart_interfaces "UART interfaces"
<br>

For detailed information about the peripheral configurations of ESP32-S3
boards, see section @ref esp32_peripherals "Common Peripherals".

### Powering Peripherals

The LCD and touch controller are power gated with a MOSFET, controlled by
GPIO1. To use it, the GPIO pin has to be set high. When using the `GC9A01` or
`CST816S` modules, the power is automatically activated.

Likewise, the +5V outputs on the external connectors for UART and I2C are
power gated by a MOSFET controlled by GPIO2. The +5V pin on the flat flex
connector has a direct connection and is not switched.
If the power pin on the UART and I2C connectors is used by your application,
make sure to set the `POWER_EXTERNAL_5V_PIN` GPIO.

### Board Pinout

The board schematic and additional documentation can be found on the
[Elecrow Wiki](https://www.elecrow.com/wiki/CrowPanel_1.28inch-HMI_ESP32_Rotary_Display.html).

## Flashing the Device

Flashing RIOT is quite easy. The board has a custom USB cable with a JST
connector with to be used for the reset/boot/flash logic. Just connect the
board to your host computer and run:

```shell
BOARD=elecrow-esp32s3-rotary-lcd-128 make flash ...
```

For detailed information about ESP32-S3 as well as configuring and compiling
RIOT for ESP32-S3 boards, see @ref esp32_riot.
