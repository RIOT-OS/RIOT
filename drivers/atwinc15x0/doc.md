/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_atwinc15x0 ATWINC15x0 WiFi module driver
 * @ingroup     drivers_netdev
 * @brief       Network device driver for the Microchip ATWINC15x0 WiFi Module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>

### Introduction

This module implements a `netdev` device driver for the Microchip ATWINC15x0
WiFi module. The ATWINC15x0 WiFi module is widely used as WiFi interface on
various boards and extension boards. Examples are:

- [Adafruit Feather M0 WiFi](https://learn.adafruit.com/adafruit-feather-m0-wifi-atwinc1500/)
  (also supported by [RIOT](https://doc.riot-os.org/group__boards__feather-m0.html))
- [ATWIN1500-XPRO](https://www.microchip.com/DevelopmentTools/ProductDetails/ATWINC1500-XPRO)
  for the extension of Atmel Xplained PRO boards (also supported by RIOT)
- [Adafruit ATWINC1500 WiFi breakout](https://learn.adafruit.com/adafruit-atwinc1500-wifi-module-breakout)
- [Watterott ATWINC1500-Breakout](https://github.com/watterott/ATWINC1500-Breakout)

Since the ATWINC15x0 WiFi module is also the only module supported by the
standard [Arduino WiFi101 library](https://github.com/arduino-libraries/WiFi101),
there are a number of Arduino shields with the ATWINC15x0 WiFi module, e.g.
[Adafruit WINC1500 WiFi Shield for Arduino](https://learn.adafruit.com/adafruit-winc1500-wifi-shield-for-arduino).

Using this 'netdev' driver together with an ATWINC15x0 WiFi module enables
the connection via WiFi in RIOT.

### Microchip ATWINC15x0 WiFi module

The ATWINC15x0 WiFi module is a low-power IEEE 802.11 b/g/n module with a
bandwidth of 20 MHz in the 2,4 GHz ISM band. It supports the IEEE 802.11i/WPA2
personal and enterprise modes. In enterprise mode it supports

- EAP-TLS
- EAP-PEAPv0/1 with TLS
- EAP-TTLSv0 with MSCHAPv2
- EAP-PEAPv0/1 with MSCHAPv2

The WiFi module ATWINC15x0 is connected via SPI and supports SPI clock speeds
of up to 48 MHz. Although the module also has an I2C and a UART interface,
these interfaces are only used for debugging.

The ATWINC15x0 WiFi module implements a complete TCP/IP procotol stack which is
not used in RIOT.

### Driver Details

The ATWINC15x0 WiFi `netdev` driver doesn't directly use the ATWINC15x0 WiFi
module. Instead, it uses the ATWINC15x0 vendor driver
[WINC1500 Wifi](http://asf.atmel.com/docs/latest/sam4s/html/group__winc1500__group.html).
from Microchip's Advanced Software Framework. For that purpose, the
[Arduino WiFi101 library](https://github.com/arduino-libraries/WiFi101) which
also includes this vendor library is used as a package.

Although the ATWINC15x0 vendor driver is complex and quite large, there is
no other way to do it because the ATWINC15x0 WiFi module's programming
interface is not documented. At the very least, the ATWINC15x0 WiFi `netdev`
driver uses only those parts of the vendor driver that are absolutely
necessary.

The ATWINC15x0 vendor driver consists of several parts:

- the M2M WiFi library, which implements high-level WiFi connection management
  Functions
- the HIF (Host Interface) library, which implements the interface with the
  ATWINC15x0 module
- the BSP (Bus Support Package) as an abstraction layer for platform-specific
  Functions
- the bus wrapper interface for access to the SPI

In addition to these parts, the ATWINC15x0 vendor driver contains a
socket library and a library for accessing the SPI flash of the ATWINC15x0
module, but these are not used in RIOT.

The ATWINC15x0 WiFi `netdev` driver replaces the BSP and the Bus Wrapper
Interface by RIOT specific parts and interacts with ATWINC15x0 vendor driver
as shown in the following figure.

```
+-----------------+        +---------------+   +----------+    +------------+
| RIOT ATWINC15x0 |        |   ATWINC15x0  |   | RIOT Bus |    | ATWINC15x0 |
|  netdev driver  |        | vendor driver |   |  Wrapper |    |   module   |
+-----------------+        +---------------+   +----------+    +------------+
        |                          |                |                |
        |      m2m_wifi_func       |    command     |  SPI(command)  |
        |------------------------->|--------------->|--------------->|
        |          return          |                |                |
        |<-------------------------|                |                |
        |                          |                |                |
        |                          |                |                |
        |                          |                |                |
        |<------------------------------- IRQ -----------------------|
        |                          |                |                |
        |  m2m_wifi_handle_events  |     request    |  SPI(request)  |
        |------------------------->|--------------->|--------------->|
        |                          |     response   | SPI(response)  |
        |    atwinc15x0_wifi_cb    |<---------------|<---------------|
        |<-------------------------|                |                |
        |                          |                |                |
        |                          |                |                |
        |                          |                |                |
        |                          |                |                |
        |<------------------------------- IRQ -----------------------|
        |                          |                |                |
        |  m2m_wifi_handle_events  |     request    |  SPI(request)  |
        |------------------------->|--------------->|--------------->|
        |                          |     response   | SPI(response)  |
        |    atwinc15x0_eth_cb     |<---------------|<---------------|
        |<-------------------------|                |                |
```

As shown, the ATWINC15x0 WiFi module operates asynchronously. This means
that when an API function of the ATWINC15x0 driver is called, it simply
sends a command to the ATWINC15x0 WiFi module and returns immediately.
The command is then executed asynchronously by the ATWINC15x0 WiFi module.
As soon as the execution of the command is completed or one or more events
are pending, the module interrupts the host. Since the ISR is executed in
the interrupt context, the ATWINC15x0 'netdev' driver simply indicates
that an interrupt has occurred. The interrupt is handled later in the
thread context by calling 'm2m_wifi_handle_events'. This in turn fetches
available information from the ATWINC15x0 WiFi module and then calls
various callback functions according to the state, all of which are
executed in the thread context.

## Configuration

The ATWINC15x0 'netdev' driver requires the configuration ATWINC15x0 WiFi
module as WiFi settings:

### Hardware Configuration

The default configuration is defined in atwinc15x0_params.h and can be
overridden either by the board definition or by environment variables in
make command line.

Symbol                         | Default          | Description
:------------------------------|:-----------------|:---------------------
`ATWINC15X0_PARAM_SPI`         | `SPI_DEV(0)`     | Used SPI device
`ATWINC15X0_PARAM_SPI_CLK`     | `SPI_CLK_10MHZ`  | Used SPI clock speed
`ATWINC15X0_PARAM_SSN_PIN`     | `GPIO_PIN(1, 4)` | SPI slave select pin
`ATWINC15X0_PARAM_RESET_PIN`   | `GPIO_PIN(4, 3)` | `RESET` pin
`ATWINC15X0_PARAM_IRQ_PIN`     | `GPIO_PIN(7, 4)` | `IRQ` pin
`ATWINC15X0_PARAM_CHIP_EN_PIN` | `GPIO_UNDEF`     | `CHIP_EN` pin
`ATWINC15X0_PARAM_WAKE_PIN`    | `GPIO_UNDEF`     | WAKE pin

### WiFi Configuration

At the moment only WPA2 Personal Mode is supported. The required settings are:

Parameter | Default   | Description
:---------|:----------|:------------
WIFI_SSID | "RIOT_AP" | SSID of the AP to be used.
WIFI_PASS | -         | Passphrase used for the AP as clear text (max. 64 chars).

The following example shows the make command with the setting of different GPIOs and the WiFi parameters.
```
USEMODULE='atwinc15x0' \
CFLAGS='-DWIFI_SSID=\"ssid\" -DWIFI_PASS=\"pass\" \
        -DATWINC15X0_PARAM_SSN_PIN=GPIO_PIN\(1,6\) \
        -DATWINC15X0_PARAM_RESET_PIN=GPIO_PIN\(1,4\) \
        -DATWINC15X0_PARAM_IRQ_PIN=GPIO_PIN\(0,8\)' \
make BOARD=... -C examples/gnrc_networking flash term
```

*/
