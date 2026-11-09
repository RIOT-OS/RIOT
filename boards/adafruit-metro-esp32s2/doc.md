<!--
SPDX-FileCopyrightText: 2026 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards_adafruit_metro_esp32s2 Adafruit Metro ESP32-S2
@ingroup    boards_esp32s2
@brief      Support for the Adafruit Metro ESP32-S2 board
@author     Gunar Schorcht <gunar@schorcht.net>

\section adafruit_metro_esp32s2 Adafruit Metro ESP32-S2

## Overview

The [Adafruit Metro ESP32-S2](https://www.adafruit.com/product/4775) is an
ESP32-S2 board in the Arduino UNO R3 form factor with 4 MB Flash and
2 MB PSRAM.

<img src="https://cdn-shop.adafruit.com/640x480/4775-06.jpg" alt="Adafruit Metro ESP32-S2" width=600px />

The main features of the board are:

- ESP32-S2 SoC with 2.4 GHz WiFi 802.11b/g/n
- ESP32-S2-WROVER module with 4 MByte Flash and 2 MByte PSRAM
- Arduino UNO R3 compatible headers including ICSP header
- Red LED and RGB LED (NeoPixel)
- STEMMA QT / Qwiic I2C connector
- MAX17048 LiPo battery monitor
- LiPo battery charger
- Native USB-C connected to the USB OTG interface of the ESP32-S2
- DC barrel jack (6 V ... 12 V)
- JTAG pads and Debug UART header

## Hardware

### MCU

Most features of the board are provided by the ESP32-S2 SoC. For detailed
information about the ESP32-S2 SoC variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

The board uses an ESP32-S2-WROVER module with 4 MByte Flash and 2 MByte
PSRAM (CPU model `esp32s2_wrover_n4r2`). The PSRAM can be used with the
module `esp_spi_ram`.

### Board Configuration

The default board configuration follows the Arduino UNO R3 compatible pinout
of the board and provides:

- 6 x ADC channels
- 2 x DAC channels
- 2 x SPI
- 1 x I2C
- 2 x UART
- 2 x PWM, 6 channels in total
- 1 x LED
- 1 x RGB LED (NeoPixel)

For flexibility, some GPIOs are used in different peripheral configurations,
but they can only be used for one peripheral at a time. For example, GPIO42
is used as LED and as SCK signal of SPI_DEV(1).

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
LED0            | GPIO42 | D13 | Red LED | |
WS281X          | GPIO45 | | RGB LED NeoPixel | |
ADC_LINE(n)     | GPIO17, GPIO18, GPIO1, GPIO2, GPIO3, GPIO4 | A0 ... A5 | A0 and A1 are ADC2 channels | \ref esp32_adc_channels "ADC Channels"
DAC_LINE(n)     | GPIO17, GPIO18 | A0, A1 | | \ref esp32_dac_channels "DAC Channels"
PWM_DEV(0)      | GPIO8, GPIO10, GPIO11, GPIO14 | D3, D5, D6, D9 | | \ref esp32_pwm_channels "PWM Channels"
PWM_DEV(1)      | GPIO15, GPIO16 | D10, D11 | | \ref esp32_pwm_channels "PWM Channels"
I2C_DEV(0) SCL  | GPIO34 | SCL | STEMMA QT | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0) SDA  | GPIO33 | SDA | STEMMA QT | \ref esp32_i2c_interfaces "I2C Interfaces"
SPI_DEV(0) SCK  | GPIO36 | ICSP | SPI2_HOST (FSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MISO | GPIO37 | ICSP | SPI2_HOST (FSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) MOSI | GPIO35 | ICSP | SPI2_HOST (FSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0) CS0  | GPIO15 | D10 | SPI2_HOST (FSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) SCK  | GPIO42 | D13 | SPI3_HOST (HSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) MISO | GPIO21 | D12 | SPI3_HOST (HSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) MOSI | GPIO16 | D11 | SPI3_HOST (HSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1) CS0  | GPIO15 | D10 | SPI3_HOST (HSPI) | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0) TxD | GPIO43 | | Debug header (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0) RxD | GPIO44 | | Debug header (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(1) TxD | GPIO6  | D1 | | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(1) RxD | GPIO5  | D0 | | \ref esp32_uart_interfaces "UART interfaces"
</center>
<br>

@note
- ADC2 is also used by the WiFi module. The ADC2 channels (A0 and A1) can't
  be used when WiFi is active.
- GPIO0, GPIO45 and GPIO46 are strapping pins. GPIO45 (NeoPixel) must not be
  pulled high during reset.
- GPIO19 and GPIO20 are used for the USB-C connector and are not available.

For detailed information about the peripheral configurations of ESP32-S2
boards, see section \ref esp32_peripherals "Common Peripherals".

### Board Pinout

The following figure shows the pinout as configured by the board definition.

<img src="https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp32/Adafruit_Metro_ESP32-S2_pinout.png" alt="Adafruit Metro ESP32-S2 Pinout" width=1200px />

The original pinout of the board is described in the
[Adafruit Learning Guide](https://learn.adafruit.com/adafruit-metro-esp32-s2/pinouts),
the schematic can be found in the
[PCB repository](https://github.com/adafruit/Adafruit-Metro-ESP32-S2-PCB).

## Flashing the Device

Since the board does not have a USB-to-Serial chip, the board is flashed via
the USB OTG interface of the ESP32-S2. Just connect the board to your host
computer and use the following command:

```shell
BOARD=adafruit-metro-esp32s2 make flash ...
```

If RIOT is already running on the board and the STDIO is realized via the
USB interface, the board automatically enters the download mode before
flashing.

@note The board is shipped with the TinyUF2 bootloader and a factory
application that use the native USB interface. Flashing RIOT overwrites the
UF2 bootloader. If the board does not enter the download mode automatically,
the programmer cannot connect to the board and flashing is aborted with a
timeout. In this case, restart the board manually in download mode by
pressing and releasing the RESET button while holding down the BOOT button.

Alternatively, an external USB-to-Serial adapter can be connected to TxD
(GPIO43) and RxD (GPIO44) of the Debug header. Before RIOT can be flashed,
the board has to be switched to download mode as described above.

For detailed information about ESP32-S2 as well as configuring and compiling
RIOT for ESP32-S2 boards, see \ref cpu_esp32.

## Using STDIO

Since the board does not have a USB-to-Serial chip, the USB OTG interface
with the USBUS stack is used by default for the STDIO (module
`stdio_cdc_acm`) which provides an USB CDC ACM interface.

If the tinyUSB stack is used instead, implicitly the module
`stdio_tinyusb_cdc_acm` is used for the STDIO via the USB CDC ACM interface.

Alternatively, the UART interface of the Debug header could be used with an
external USB-to-Serial adapter. Simply add `stdio_uart` to the list of used
modules for this purpose:

```shell
BOARD=adafruit-metro-esp32s2 USEMODULE=stdio_uart make flash ...
```
