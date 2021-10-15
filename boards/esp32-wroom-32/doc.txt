/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_esp32_wroom-32 Generic ESP32-WROOM-32 boards
 * @ingroup     boards_esp32
 * @brief       Support for generic ESP32-WROOM-32 boards
 * @author      Gunar Schorcht <gunar@schorcht.net>

\section esp32_wroom_32 ESP32-WROOM-32 based boards

## Table of Contents {#esp32_wroom_32_toc}

1. [Overview](#esp32_wroom_32_overview)
2. [Hardware](#esp32_wroom_32_hardware)
    1. [MCU](#esp32_wroom_32_mcu)
    2. [Board Configuration](#esp32_wroom_32_board_configuration)
    3. [Board Pinout](#esp32_wroom_32_pinout)
    4. [Optional Hardware Configurations](#esp32_wroom_32_optional_hardware)
3. [Flashing the Device](#esp32_wroom_32_flashing)

## Overview {#esp32_wroom_32_overview}

This board definition covers not just a single board, but rather a large set
of generic boards that use an ESP32-WROOM-32 module and simply break out all
GPIOs to external pads without having any special hardware or interfaces
on-board. Examples are Espressif's ESP32-DevKitC or NodeMCU-ESP32S and a
large number of clones.

\image html "https://dl.espressif.com/dl/schematics/pictures/esp32-devkitc-v4-front.jpg" "Espressif ESP32-DevKitC V4" width=400px

[Back to table of contents](#esp32_wroom_32_toc)

## Hardware {#esp32_wroom_32_hardware}

This section describes

- the [MCU](#esp32_wroom_32_mcu),
- the default [board configuration](#esp32_wroom_32_board_configuration),
- [optional hardware configurations](#esp32_wroom_32_optional_hardware),
- the [board pinout](#esp32_wroom_32_pinout).

[Back to table of contents](#esp32_wroom_32_toc)

### MCU {#esp32_wroom_32_mcu}

Most features of the board are provided by the ESP32 SoC. For detailed
information about the ESP32, see section \ref esp32_mcu_esp32 "MCU ESP32".

[Back to table of contents](#esp32_wroom_32_toc)

### Board Configuration {#esp32_wroom_32_board_configuration}

Generic ESP32-WROOM-32 boards do not have special hardware on board and all
GPIOs are simply broken out for flexibility. Therefore, the board
configuration is the most flexible one with provides:

18 x ADC channels at maximum
2 x DAC channels at maximum
2 x SPI at maximum
1 x I2C at maximum
2 x UART

Since all GPIOs have broken out, GPIOs can be used for different purposes
in different applications. For flexibility, GPIOs can be listed in various
peripheral configurations. For example, GPIO13 is used in the ADC channel
definition and the definition of the MOSI signal of SPI_DEV(0).

This is possible because GPIOs are only used for a specific peripheral
interface when

- the corresponding peripheral module is used, e.g. `periph_i2c`, or
- a corresponding init function is called z. `adc_init`, `dac_init` and
  `pwm_init` or
- The corresponding peripheral interface is used for the first time,
  e.g. `spi_aqcuire.

That is, the purpose for which a GPIO is used depends on which module
or function is used first.

For example, if module periph_i2c is not used, the GPIOs listed in I2C
configuration can be used for the other purposes.

The following table shows the default board configuration, which is sorted
according to the defined functionality of GPIOs. This configuration can be
overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | GPIOs  | Remarks |Configuration
:---------------|:-------|:--------|:----------------------------------
BUTTON0         | GPIO0  | | |
ADC             | GPIO0, GPIO2, GPIO4, GPIO12, GPIO13,\n GPIO14, GPIO15, GPIO25, GPIO26, GPIO27,\n GPIO32, GPIO33, GPIO34, GPIO35, GPIO36,\n GPIO39 | | see \ref esp32_adc_channels "ADC Channels"
DAC             | GPIO25, GPIO26 | | \ref esp32_dac_channels "refer"
PWM_DEV(0)      | GPIO0, GPIO2, GPIO4, GPIO16, GPIO17 | - | \ref esp32_pwm_channels "DAC Channels"
PWM_DEV(1)      | GPIO27, GPIO32, GPIO33 | - | \ref esp32_pwm_channels "PWM Channels"
I2C_DEV(0):SDA  | GPIO21 | | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0):SCL  | GPIO22 | | \ref esp32_i2c_interfaces "I2C Interfaces"
SPI_DEV(0):CLK  | GPIO18 | VSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MISO | GPIO19 | VSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MOSI | GPIO23 | VSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):CS0  | GPIO5  | VSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1):CLK  | GPIO14 | HSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1):MISO | GPIO12 | HSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1):MOSI | GPIO13 | HSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(1):CS0  | GPIO15 | HSPI is used | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0):TxD | GPIO1  | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0):RxD | GPIO3  | Console (configuration is fixed) | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(1):TxD | GPIO10 | not available in **qout** and **qio** flash mode | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(1):RxD | GPIO9  | not available in **qout** and **qio** flash mode | \ref esp32_uart_interfaces "UART interfaces"
</center>
\n
@note
- The configuration of ADC channels contains all ESP32 GPIOs that can be
  used as ADC channels.
- The configuration of DAC channels contains all ESP32 GPIOs that can be used
  as DAC channels.
- GPIO9 and GIOP10 can only be used in **dout** and **dio**
  \ref esp32_flash_modes "flash modes".

For detailed information about the configuration of ESP32 boards, see
section \ref esp32_peripherals "Common Peripherals".

[Back to table of contents](#esp32_wroom_32_toc)

### Optional Hardware Configurations {#esp32_wroom_32_optional_hardware}

MRF24J40-based IEEE 802.15.4 radio modules and ENC28J60-based Ethernet
network interface modules have been tested with the board. You could use
the following code in your \ref esp32_application_specific_configurations
"application-specific configuration" to use such modules:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef BOARD_ESP32_WROOM-32

#if MODULE_MRF24J40
#define MRF24J40_PARAM_CS       GPIO16       /* MRF24J40 CS signal    */
#define MRF24J40_PARAM_RESET    GPIO17       /* MRF24J40 RESET signal */
#define MRF24J40_PARAM_INT      GPIO34       /* MRF24J40 INT signal   */
#define MRF24J40_PARAM_SPI_CLK  SPI_CLK_1MHZ /* SPI clock frequency */
#endif

#if MODULE_ENC28J80
#define ENC28J80_PARAM_CS       GPIO32      /* ENC28J80 CS signal    */
#define ENC28J80_PARAM_RESET    GPIO33      /* ENC28J80 RESET signal */
#define ENC28J80_PARAM_INT      GPIO35      /* ENC28J80 INT signal   */
#endif

#endif
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For other parameters, the default values defined by the drivers can be used.

@note The **RESET** signal of MRF24J40 and ENC28J60 based modules can also
be connected to the **RST** pin of the board (see \ref esp32_wroom_32_pinout
"pinout") to keep the configured GPIO free for other purposes.

[Back to table of contents](#esp32_wroom_32_toc)

### Board Pinout {#esp32_wroom_32_pinout}

The following figure shows the pinout of the defined default configuration
for the EPS32-DevKitC board as an example of generic ESP32-WROOM-32 boards.
The light green GPIOs are not used by configured on-board hardware components
and can be used for any purpose. However, if optional off-board hardware
modules are used, these GPIOs may also be occupied, see
section \ref esp32_wroom_32_board_configuration for more information.

The corresponding board schematics can be found her [here]
(https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch-20180607a.pdf)

@image html "https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp32/ESP32-WROOM-32_pinouts.png" "EPS32-DevKitC V4 Pinout"

[Back to table of contents](#esp32_wroom_32_toc)

## Flashing the Device {#esp32_wroom_32_flashing}

Flashing RIOT is quite easy. The board has a Micro-USB connector with
reset/boot/flash logic. Just connect the board to your host computer
and type using the programming port:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make flash BOARD=esp32-wroom-32 ...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
For detailed information about ESP32 as well as configuring and compiling
RIOT for ESP32 boards, see \ref esp32_riot.

[Back to table of contents](#esp32_wroom_32_toc)
 */
