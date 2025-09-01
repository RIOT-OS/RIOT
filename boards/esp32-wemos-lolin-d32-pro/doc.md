<!--
SPDX-FileCopyrightText: 2018 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards_esp32_wemos-lolin-d32-pro Wemos LOLIN D32 Pro
@ingroup    boards_esp32
@brief      Support for Wemos LOLIN D32 Pro
@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32_wemos_lolin_d32_pro Wemos LOLIN D32 Pro

## Table of Contents {#esp32_wemos_lolin_d32_pro_toc}

1. [Overview](#esp32_wemos_lolin_d32_pro_overview)
2. [Hardware](#esp32_wemos_lolin_d32_pro_hardware)
    1. [MCU](#esp32_wemos_lolin_d32_pro_mcu)
    2. [Board Configuration](#esp32_wemos_lolin_d32_pro_board_configuration)
    3. [Board Pinout](#esp32_wemos_lolin_d32_pro_pinout)
    4. [Optional Hardware Configurations](#esp32_wemos_lolin_d32_pro_optional_hardware)
3. [Flashing the Device](#esp32_wemos_lolin_d32_pro_flashing)

## Overview {#esp32_wemos_lolin_d32_pro_overview}

Wemos LOLIN D32 Pro is a development board that uses the ESP32-WROVER module
which has a built-in 4 MByte SPI RAM. Most important features of the board are

- Micro-SD card interface
- TFT display interface
- SPI RAM 4 MByte

Wemos LOLIN D32 Pro belongs to the class of general purpose boards where most
ESP32 pins are broken out for easier access.

@image html
"https://docs.wemos.cc/en/latest/_static/boards/d32_pro_v2.0.0_1_16x16.jpg" "Wemos LOLIN D32 PRO" width=400px

[Back to table of contents](#esp32_wemos_lolin_d32_pro_toc)

## Hardware {#esp32_wemos_lolin_d32_pro_hardware}

This section describes

- the [MCU](#esp32_wemos_lolin_d32_pro_mcu),
- the default [board configuration](#esp32_wemos_lolin_d32_pro_board_configuration),
- [optional hardware configurations](#esp32_wemos_lolin_d32_pro_optional_hardware),
- the [board pinout](#esp32_wemos_lolin_d32_pro_pinout).

[Back to table of contents](#esp32_wemos_lolin_d32_pro_toc)

### MCU {#esp32_wemos_lolin_d32_pro_mcu}

Most features of the board are provided by the ESP32 SoC. For detailed
information about the ESP32, see section \ref esp32_mcu_esp32 "MCU ESP32".

[Back to table of contents](#esp32_wemos_lolin_d32_pro_toc)

### Board Configuration {#esp32_wemos_lolin_d32_pro_board_configuration}

The board for the Wemos LOLIN D32 Pro has the following on-board components:

- 1 x LED
- 1 x Micro SD card interface
- 1 x TFT display connector

The following table shows the default board configuration, which is sorted
according to the defined functionality of GPIOs. This configuration can be
overridden by an \ref esp32_application_specific_configurations
"application-specific configuration".

<center>
Pin    | Default Configuration* | Optional Configuration | Remarks / Prerequisites | Configuration
:------|:-----------------|:--------------------|:--------------|:-------------
GPIO22 | I2C_DEV(0):SCL   | | | \ref esp32_i2c_interfaces "I2C Interfaces"
GPIO21 | I2C_DEV(0):SDA   | | | \ref esp32_i2c_interfaces "I2C Interfaces"
GPIO18 | SPI_DEV(0):SCK   | | | \ref esp32_spi_interfaces "SPI Interfaces"
GPIO19 | SPI_DEV(0):MISO  | | | \ref esp32_spi_interfaces "SPI Interfaces"
GPIO23 | SPI_DEV(0):MOSI  | | | \ref esp32_spi_interfaces "SPI Interfaces"
GPIO5  | SPI_DEV(0):CS0 / LED0 | | | \ref esp32_spi_interfaces "SPI Interfaces"
GPIO4  | SPI_DEV(0):CS1   | SD Card CS | when module \ref drivers_sdcard_spi "sdcard_spi" is used | \ref esp32_spi_interfaces "SPI Interfaces"
GPIO1  | UART_DEV(0):TxD  | | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
GPIO3  | UART_DEV(0):RxD  | | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
GPIO36 | ADC_LINE(0) | | | \ref esp32_adc_channels "ADC Channels"
GPIO39 | ADC_LINE(1) | | | \ref esp32_adc_channels "ADC Channels"
GPIO34 | ADC_LINE(2) | | | \ref esp32_adc_channels "ADC Channels"
GPIO35 | ADC_LINE(3) | | VBat measurement (GPIO is not broken out) | \ref esp32_adc_channels "ADC Channels"
GPIO32 | ADC_LINE(4) | TFT_LED  | when TFT is connected | \ref esp32_adc_channels "ADC Channels"
GPIO33 | ADC_LINE(5) | TFT_RESET| when TFT is connected | \ref esp32_adc_channels "ADC Channels"
GPIO25 | DAC_LINE(0) | | | \ref esp32_dac_channels "DAC Channels"
GPIO26 | DAC_LINE(1) | | | \ref esp32_dac_channels "DAC Channels"
GPIO0  | PWM_DEV(0):0| MRF24J40/ENC28J60 RESET | when module \ref drivers_mrf24j40 "mrf24j40"/\ref drivers_enc28j60 "enc28j60" is used | \ref esp32_pwm_channels "PWM Channels"
GPIO2  | PWM_DEV(0):1| MRF24J40/ENC28J60 CS    | when module \ref drivers_mrf24j40 "mrf24j40"/\ref drivers_enc28j60 "enc28j60" is used | \ref esp32_pwm_channels "PWM Channels"
GPIO13 | -                 | MRF24J40/ENC28J60 INT   | when module \ref drivers_mrf24j40 "mrf24j40"/\ref drivers_enc28j60 "enc28j60" is used | |
GPIO15 | -  | | | |
GPIO12 | -  | TS_CS  | when TFT is connected | |
GPIO14 | -  | TFT_CS | when TFT is connected | |
GPIO27 | -  | TFT_DC | when TFT is connected | |
</center>

\b * Default configuration cannot be used or is not available at all when
optional configuration is used. For example, when the TFT is connected,
GPIO32 is used as **TFT_LED** signal and ADC_LINE(4) is not available.

@note When the TFT display is connected, add the following line to the
makefile of the application to enable the according default board and
peripheral configuration:
\n
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_lolin_tft
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For detailed information about the configuration of ESP32 boards, see section \ref esp32_peripherals "Common Peripherals".

[Back to table of contents](#esp32_wemos_lolin_d32_pro_toc)

### Optional Hardware Configurations {#esp32_wemos_lolin_d32_pro_optional_hardware}

MRF24J40-based IEEE 802.15.4 radio modules and ENC28J60-based Ethernet
network interface modules have been tested with the board. You could use
the following code in your \ref esp32_application_specific_configurations
"application-specific configuration" to use such modules:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef BOARD_ESP32_WEMOS_LOLIN_D32_PRO

#if MODULE_MRF24J40
#define MRF24J40_PARAM_CS       GPIO15      /* MRF24J40 CS signal    */
#define MRF24J40_PARAM_RESET    GPIO2       /* MRF24J40 RESET signal */
#define MRF24J40_PARAM_INT      GPIO13      /* MRF24J40 INT signal   */
#endif

#if MODULE_ENC28J80
#define ENC28J80_PARAM_CS       GPIO15      /* ENC28J80 CS signal    */
#define ENC28J80_PARAM_RESET    GPIO2       /* ENC28J80 RESET signal */
#define ENC28J80_PARAM_INT      GPIO13      /* ENC28J80 INT signal   */
#endif

#endif
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For other parameters, the default values defined by the drivers can be used.

@note
- Only a few GPIOs are available for external hardware on the
  Wemos LOLIN D32 PRO. Therefore, MRF24J40 and ENC28J60 based modules use
  the same GPIOs and only one of these modules can be used simultaneously.
- The **RESET** signal of MRF24J40 and ENC28J60 based modules can also be
  connected to the **RST** pin of the board (see
  \ref esp32_wemos_lolin_d32_pro_pinout_img "pinout") to keep the configured
  GPIO free for other purposes.

[Back to table of contents](#esp32_wemos_lolin_d32_pro_toc)

### Board Pinout {#esp32_wemos_lolin_d32_pro_pinout}

The following picture shows the pinout of WEMOS LOLIN D32 PRO board as
defined by the default board configuration. The light green GPIOs are
not used by configured on-board hardware components and can be used for
any purpose. However, if optional off-board hardware modules are used,
these GPIOs may also be occupied, see section
\ref esp32_wemos_lolin_d32_pro_board_configuration for more information.

The corresponding board schematic can be found
[here](https://docs.wemos.cc/en/latest/_static/files/sch_d32_pro_v2.0.0.pdf).

\anchor esp32_wemos_lolin_d32_pro_pinout_img
@image html "https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp32/Wemos_LOLIN_D32_PRO_pinout.png" "Wemos LOLIN D32 PRO pinout"

[Back to table of contents](#esp32_wemos_lolin_d32_pro_toc)

## Flashing the Device {#esp32_wemos_lolin_d32_pro_flashing}

Flashing RIOT is quite easy. The board has a Micro-USB connector with
reset/boot/flash logic. Just connect the board to your host computer
using the programming port and type:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make flash BOARD=esp32-wemos-lolin-d32-pro ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For detailed information about ESP32 as well as configuring and compiling
RIOT for ESP32 boards, see \ref esp32_riot.

[Back to table of contents](#esp32_wemos_lolin_d32_pro_toc)
