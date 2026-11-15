<!--
SPDX-FileCopyrightText: 2026 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards_adafruit_metro_esp32s3 Adafruit Metro ESP32-S3
@ingroup    boards_esp32s3
@brief      Support for the Adafruit Metro ESP32-S3 board
@author     Gunar Schorcht <gunar@schorcht.net>

\section adafruit_metro_esp32s3 Adafruit Metro ESP32-S3

## Overview

The [Adafruit Metro ESP32-S3](https://www.adafruit.com/product/5500) is an
ESP32-S3 board in the Arduino UNO R3 form factor with 16 MB Flash and
8 MB PSRAM.

The main features of the board are:

- ESP32-S3 SoC with 2.4 GHz WiFi 802.11b/g/n and Bluetooth 5 (LE)
- 16 MByte Quad SPI Flash
- 8 MByte Octal SPI PSRAM
- Arduino UNO R3 compatible headers including ICSP header
- microSD card slot (SPI)
- Red LED and RGB LED (NeoPixel)
- STEMMA QT / Qwiic I2C connector
- MAX17048 LiPo battery monitor (currently not supported)
- LiPo battery charger with JST PH connector
- Native USB-C with USB Serial/JTAG
- DC barrel jack (6 V ... 12 V)
- 2x5 1.27 mm JTAG header and Debug UART header

## Hardware

### MCU

Most features of the board are provided by the ESP32-S3 SoC. For detailed
information about the ESP32-S3 SoC variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

The board uses an ESP32-S3 module with Quad SPI Flash and Octal SPI PSRAM
(CPU model `esp32s3_wroom_1x_n16r8`). Since the Octal SPI PSRAM is used,
GPIO33 ... GPIO37 are not available. When the module `esp_spi_ram` is used,
the feature `esp_spi_oct` is enabled automatically.

### Board Configuration

The default board configuration follows the Arduino UNO R3 compatible pinout
of the board and provides:

- 18 x ADC channels at maximum
- 2 x SPI
- 1 x I2C
- 2 x UART
- 2 x PWM, 6 channels in total
- 1 x LED
- 1 x RGB LED (NeoPixel)
- 1 x microSD card slot

For flexibility, some GPIOs are used in different peripheral configurations,
but they can only be used for one peripheral at a time. For example, GPIO13
is used as LED, as ADC channel and as SCK signal of SPI_DEV(1).

This is possible because GPIOs are only used for a specific peripheral
interface when either

- the corresponding peripheral module is used, e.g. `periph_i2c`, or
- the corresponding init function is called, e.g. `adc_init`.

That is, the purpose for which a GPIO is used depends on which module
or function is used first.

The following table shows the default board configuration, which is sorted
according to the defined functionality of GPIOs. This configuration can be
overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | GPIOs  | Arduino | Remarks | Configuration
:---------------|:-------|:--------|:--------|:----------------------------------
BTN0            | GPIO0  | | labeled as BOOT button | |
LED0            | GPIO13 | D13 | Red LED | |
WS281X          | GPIO46 | | RGB LED NeoPixel | |
ADC_LINE(n)     | GPIO14, GPIO15, GPIO16, GPIO17, GPIO18, GPIO1 | A0 ... A5 | A0 ... A4 are ADC2 channels | \ref esp32_adc_channels "ADC Channels"
ADC_LINE(n)     | GPIO2 ... GPIO13 | D2 ... D13 | D11 ... D13 are ADC2 channels | \ref esp32_adc_channels "ADC Channels"
PWM_DEV(0)      | GPIO3, GPIO5, GPIO6, GPIO9 | D3, D5, D6, D9 | | \ref esp32_pwm_channels "PWM Channels"
PWM_DEV(1)      | GPIO10, GPIO11 | D10, D11 | | \ref esp32_pwm_channels "PWM Channels"
I2C_DEV(0) SCL  | GPIO48 | SCL | STEMMA QT, MAX17048 | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0) SDA  | GPIO47 | SDA | STEMMA QT, MAX17048 | \ref esp32_i2c_interfaces "I2C Interfaces"
SPI_DEV(0) SCK  | GPIO39 | ICSP | microSD card, SPI2_HOST (FSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MISO | GPIO21 | ICSP | microSD card, SPI2_HOST (FSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MOSI | GPIO42 | ICSP | microSD card, SPI2_HOST (FSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) CS0  | GPIO45 | | microSD card CS | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) SCK  | GPIO13 | D13 | SPI3_HOST | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) MISO | GPIO12 | D12 | SPI3_HOST | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) MOSI | GPIO11 | D11 | SPI3_HOST | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) CS0  | GPIO10 | D10 | SPI3_HOST | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0) TxD | GPIO43 | | Debug header (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0) RxD | GPIO44 | | Debug header (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(1) TxD | GPIO40 | D1 | | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(1) RxD | GPIO41 | D0 | | \ref esp32_uart_interfaces "UART interfaces"
</center>
<br>

@note
- ADC2 is also used by the WiFi module. The ADC2 channels (A0 ... A4 and
  D11 ... D13) can't be used when WiFi is active.
- GPIO0, GPIO3, GPIO45 and GPIO46 are strapping pins, see
  \ref esp32_gpio_pins_esp32s3_strapping. GPIO45 (SD_CS) and GPIO46
  (NeoPixel) must not be pulled high during reset.
- The 2x5 JTAG header uses GPIO39 ... GPIO42, which are also used for
  SPI_DEV(0) and UART_DEV(1). By default, the built-in USB Serial/JTAG
  interface is used for debugging instead.

For detailed information about the peripheral configurations of ESP32-S3
boards, see section \ref esp32_peripherals "Common Peripherals".

### Board Pinout

The following figure shows the pinout as configured by the board definition.

<img src="https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp32/Adafruit_Metro_ESP32-S3_pinout.png" alt="Adafruit Metro ESP32-S3 Pinout" width=1200px />

The original pinout of the board is described in the
[Adafruit Learning Guide](https://learn.adafruit.com/adafruit-metro-esp32-s3/pinouts),
the schematic can be found on the
[Downloads](https://learn.adafruit.com/adafruit-metro-esp32-s3/downloads) page.

### microSD Card

The microSD card slot is connected to SPI_DEV(0). If the module `mtd` is
used, the module `mtd_sdcard_default` is enabled automatically. If the module
`vfs_default` is used, the SD card is mounted with FAT file system support.

## Flashing the Device

Since the board does not have a USB-to-Serial chip, the easiest way to flash
the board is using the USB Serial/JTAG interface. Just connect the board to
your host computer and use the following command:

```shell
BOARD=adafruit-metro-esp32s3 make flash ...
```

@note The board is shipped with the TinyUF2 bootloader and a factory
application that uses the native USB interface. Flashing RIOT overwrites the
UF2 bootloader. If the USB Serial/JTAG interface is not available, the
programmer cannot connect to the board and flashing is aborted with a
timeout. In this case, restart the board manually in download mode by
pressing and releasing the RESET button while holding down the BOOT button.
In download mode, the USB Serial/JTAG interface is always available.

Alternatively, an external USB-to-Serial adapter can be connected to TxD
(GPIO43) and RxD (GPIO44) of the Debug header. Before RIOT can be flashed,
the board has to be switched to download mode as described above.

For detailed information about ESP32-S3 as well as configuring and compiling
RIOT for ESP32-S3 boards, see \ref cpu_esp32.

## Using STDIO

Since the board does not have a USB-to-Serial chip, the USB Serial/JTAG
interface is used by default for the STDIO (module `stdio_usb_serial_jtag`)
which provides an USB CDC ACM interface.

If the USB port is used by the USBUS stack or the tinyUSB stack, implicitly
the module `stdio_cdc_acm` or `stdio_tinyusb_cdc_acm` is used for the STDIO
via the USB CDC ACM interface.

Alternatively, the UART interface of the Debug header could be used with an
external USB-to-Serial adapter. Simply add `stdio_uart` to the list of used
modules for this purpose:

```shell
BOARD=adafruit-metro-esp32s3 USEMODULE=stdio_uart make flash ...
```
