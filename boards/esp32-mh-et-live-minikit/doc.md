<!--
SPDX-FileCopyrightText: 2018 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   boards_esp32_mh-et-live-minikit MH-ET LIVE MiniKit
@ingroup    boards_esp32
@brief      Support for MH-ET LIVE MiniKit for ESP32
@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32_mh_et_live_minikit MH-ET LIVE MiniKit for ESP32

## Overview

The MH-ET LIVE MiniKit for ESP32 uses the ESP32-WROOM-32 module. It is a very
interesting development kit as it uses in the stackable Wemos D1 Mini format.
Thus, all [shields for Wemos D1 mini](https://docs.wemos.cc/en/latest/d1_mini_shield/index.html)
for ESP8266 can also be used with ESP32. Examples for such shields are:

- Micro SD-Card Shield (enable module `sdcard_spi` to use it)
- MRF24J40 IEEE 802.15.4 radio Shield
- Button Shield
- RGB LED Shield
- ...

This makes it possible to create different hardware configurations without
the need for a soldering iron or a breadboard.

MH-ET LIVE MiniKit for ESP32 belongs to the class of general purpose boards
where most ESP32 pins are broken out for easier access.

<img src="https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp32/MH-ET_LIVE_D1_mini_ESP32_2.png" alt="MH-ET LIVE MiniKit for ESP32" width=250px />

This stackable platform was tested in an RIOT application with:

- [Micro SD-Card Shield](https://docs.wemos.cc/en/latest/d1_mini_shield/micro_sd.html)
- MRF24J40 IEEE 802.15.4 radio Shield (contact <gunar@schorcht.net> for more information)
- BMP180 Pressure Sensor Shield

This application is a good example how easy it is with this board to create
different hardware applications.

<img src="https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp32/MH-ET_LIVE_D1_mini_ESP32_5.png" alt="RIOT application with SD-Card, MRF24J40 Radio, and BMP180 Pressure Sensor" />

## Hardware

### MCU

Most features of the board are provided by the ESP32 SoC. For detailed
information about the ESP32, see section \ref esp32_mcu_esp32 "MCU ESP32".

### Board Configuration {#esp32_mh_et_live_minikit_board_configuration}

The following table shows the default board configuration, which is sorted
according to the defined functionality of GPIOs. This configuration can be
overridden by an \ref esp32_application_specific_configurations
"application-specific configuration".

<center>
Pin    | Default Configuration* | Optional Modules | Remarks / Prerequisites | Configuration
:------|:-------------------------|:--------------------------|:--------|:------
GPIO2  | PWM_DEV(0):0 / LED blue  | | | \ref esp32_pwm_channels "PWM Channels"
GPIO0  | PWM_DEV(0):1             | | | \ref esp32_pwm_channels "PWM Channels"
GPIO4  | PWM_DEV(0):2             | | | \ref esp32_pwm_channels "PWM Channels"
GPIO15 | PWM_DEV(0):3             | | | \ref esp32_pwm_channels "PWM Channels"
GPIO22 | I2C_DEV(0):SCL           | | | \ref esp32_i2c_interfaces "I2C Interfaces"
GPIO21 | I2C_DEV(0):SDA           | | | \ref esp32_i2c_interfaces "I2C Interfaces"
GPIO18 | SPI_DEV(0):SCK           | | | \ref esp32_spi_interfaces "SPI Interfaces"
GPIO19 | SPI_DEV(0):MISO          | | | \ref esp32_spi_interfaces "SPI Interfaces"
GPIO23 | SPI_DEV(0):MOSI          | | | \ref esp32_spi_interfaces "SPI Interfaces"
GPIO5  | SPI_DEV(0):CS0           | SD Card CS | when module \ref drivers_sdcard_spi "sdcard_spi" is used |  \ref esp32_spi_interfaces "SPI Interfaces"
GPIO1  | UART_DEV(0):TxD          | | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
GPIO3  | UART_DEV(0):RxD          | | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
GPIO9  | UART_DEV(1):TxD          | | | \ref esp32_uart_interfaces "UART interfaces"
GPIO10 | UART_DEV(1):RxD          | | | \ref esp32_uart_interfaces "UART interfaces"
GPIO34 | ADC_LINE(0)              | | | \ref esp32_adc_channels "ADC Channels"
GPIO35 | ADC_LINE(1)              | | | \ref esp32_adc_channels "ADC Channels"
GPIO36 | ADC_LINE(2)              | | | \ref esp32_adc_channels "ADC Channels"
GPIO39 | ADC_LINE(3)              | | | \ref esp32_adc_channels "ADC Channels"
GPIO25 | DAC_LINE(0)              | | | \ref esp32_dac_channels "DAC Channels"
GPIO13 | -   | | | |
GPIO12 | -   | | | |
GPIO14 | -   | | | |
GPIO16 | -   | MRF24J40 RESET | when module \ref drivers_mrf24j40 "mrf24j40" is used | |
GPIO17 | -   | MRF24J40 INT   | when module \ref drivers_mrf24j40 "mrf24j40" is used | |
GPIO26 | -   | MRF24J40 CS    | when module \ref drivers_mrf24j40 "mrf24j40" is used | |
GPIO27 | -   | | | |
GPIO32 | -   | | | |
GPIO33 | -   | | | |
</center>

\b * Default configuration cannot be used or is not available at all when
optional hardware is used.

@note
- GPIO9 and GIOP10 can only be used in **dout** and **dio**
  \ref esp32_flash_modes "flash modes".
- The **RESET** signal of MRF24J40 shield can be connected to the RST **pin**
  of the board (see \ref esp32_mh_et_live_minikit_pinout_img "pinout") to
  keep the configured GPIO free for other purposes.

For detailed information about the configuration of ESP32 boards, see
section \ref esp32_peripherals "Common Peripherals".

### Optional Hardware Configurations

ENC28J60-based Ethernet network interface modules have been tested with the
board. You could use the following code in your
\ref esp32_application_specific_configurations
"application-specific configuration" to use such a module:

```c
#if MODULE_ENC28J80 && BOARD_ESP32_MH_ET_LIVE_MINIKIT
#  define ENC28J80_PARAM_CS       GPIO14    /* ENC28J80 CS signal    */
#  define ENC28J80_PARAM_INT      GPIO33    /* ENC28J80 INT signal   */
#  define ENC28J80_PARAM_RESET    GPIO12    /* ENC28J80 RESET signal */
#endif
```

For **ENC28J80_PARAM_SPI** the default parameter defined by the driver can
be used.

@note The **RESET** signal of ENC28J60 based modules can also be connected
to the **RST** pin of the board (see \ref esp32_mh_et_live_minikit_pinout_img
"pinout") to keep the configured GPIO free for other purposes.

### Board Pinout {#esp32_mh_et_live_minikit_pinout}

The following picture shows the pinout of MH-ET LIVE MiniKit for ESP32 board
as defined by the default board configuration. The light green GPIOs are not
used by configured on-board hardware components and can be used for any
purpose. However, if optional off-board hardware modules are used, these
GPIOs may also be occupied, see section
\ref esp32_mh_et_live_minikit_board_configuration for more information.

The corresponding board schematic can be found
[here](https://i.imgur.com/EpE4dGj.jpg)

\anchor esp32_mh_et_live_minikit_pinout_img
<img src="https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp32/MH-ET_LIVE_D1_mini_ESP32_pinout.png" alt="MH-ET LIVE MiniKit for ESP32 pinout" />

## Flashing the Device

Flashing RIOT is quite easy. The board has a Micro-USB connector with a
reset/boot/flash logic. Just connect the board to your host computer using
the programming port and type:

```shell
BOARD=esp32-mh-et-live-minikit make flash ...
```

For detailed information about ESP32 as well as configuring and compiling
RIOT for ESP32 boards, see \ref esp32_riot.
