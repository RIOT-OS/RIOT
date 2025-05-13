<!--
Copyright (C) 2022 Gunar Schorcht

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
-->

@defgroup   cpu_esp32_esp32c3 ESP32-C3 family
@ingroup    cpu_esp32
@brief      Specific properties of ESP32-C3 variant (family)
@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32_riot_esp32c3  Specific properties of ESP32-C3 variant (family)

## GPIO pins {#esp32_gpio_pins_esp32c3}

ESP32-C3 has 22 GPIO pins, where a subset can be used as ADC channel and as
low-power digital input/output in deep-sleep mode, the so-called RTC GPIOs.
Some of them are used by special SoC components. The following table gives
a short overview.

<center>

Pin    | Type   | ADC / RTC  | PU / PD | Special function | Remarks
-------|:-------|:----------:|:-------:|------------------|--------
GPIO0  | In/Out | yes        | yes     | XTAL_32K_P       | -
GPIO1  | In/Out | yes        | yes     | XTAL_32K_N       | -
GPIO2  | In/Out | yes        | yes     |                  | Bootstrapping
GPIO3  | In/Out | yes        | yes     |                  | -
GPIO4  | In/Out | yes        | yes     | MTMS             | JTAG interface
GPIO5  | In/Out | yes        | yes     | MTDI             | JTAG interface
GPIO6  | In/Out | -          | yes     | MTCK             | JTAG interface
GPIO7  | In/Out | -          | yes     | MTDO             | JTAG interface
GPIO8  | In/Out | -          | yes     | -                | -
GPIO9  | In/Out | -          | yes     | -                | Bootstrapping, pulled up
GPIO10 | In/Out | -          | yes     | -                | -
GPIO11 | In/Out | -          | yes     | VDD_SPI          | not broken out
GPIO12 | In/Out | -          | yes     | Flash SDIHD      | only in `qout`and `qio`mode, see section [Flash Modes](#esp32_flash_modes)
GPIO13 | In/Out | -          | yes     | Flash SPIWP      | only in `qout`and `qio`mode, see section [Flash Modes](#esp32_flash_modes)
GPIO14 | In/Out | -          | yes     | Flash SPICS0     | -
GPIO15 | In/Out | -          | yes     | Flash SPICLK     | -
GPIO16 | In/Out | -          | yes     | Flash SPID       | -
GPIO17 | In/Out | -          | yes     | Flash SPIQ       | -
GPIO18 | In/Out | -          | yes     | -                | USB-JTAG
GPIO19 | In/Out | -          | yes     | -                | USB-JTAG
GPIO21 | In/Out | -          | yes     | UART0 RX         | Console
GPIO22 | In/Out | -          | yes     | UART0 TX         | Console

</center><br>

<b>ADC:</b> these pins can be used as ADC inputs<br>
<b>RTC:</b> these pins are RTC GPIOs and can be used in deep-sleep mode<br>
<b>PU/PD:</b> these pins have software configurable pull-up/pull-down functionality.<br>

@note GPIOs that can be used as ADC channels are also available as low power
      digital inputs/outputs in deep sleep mode.

GPIO2, GPIO8 and GPIO9 are bootstrapping pins which are used to boot ESP32-C3
in different modes:

<center>

GPIO2 | GPIO8 | GPIO9 | Mode
:----:|:-----:|-------|----------
1     | X     | 1     | SPI Boot mode to boot the firmware from flash (default mode)
1     | 1     | 0     | Download Boot mode for flashing the firmware

</center><br>

## ADC Channels {#esp32_adc_channels_esp32c3}

ESP32-C3 integrates two 12-bit ADCs (ADC1 and ADC2) with 6 channels in
total:

- ADC1 supports 5 channels: GPIO0, GPIO1, GPIO2, GPIO3 and GPIO4
- ADC2 supports 1 channel: GPIO5 or internal signals such as vdd33

The maximum number of ADC channels #ADC_NUMOF_MAX is 6.

@note
- ADC2 is also used by the WiFi module. The GPIOs connected to ADC2 are
  therefore not available as ADC channels if the modules `esp_wifi` or
  `esp_now` are used.
- Vref can be read with function #adc_line_vref_to_gpio at GPIO5.

## I2C Interfaces {#esp32_i2c_interfaces_esp32c3}

ESP32-C3 has one built-in I2C interfaces.

The following table shows the default configuration of I2C interfaces
used for ESP32-C3 boards. It can be overridden by
[application-specific configurations](#esp32_application_specific_configurations).

<center>

Device     | Signal | Pin    | Symbol        | Remarks
:----------|:-------|:-------|:--------------|:----------------
I2C_DEV(0) |        |        | `#I2C0_SPEED` | default is `I2C_SPEED_FAST`
I2C_DEV(0) | SCL    | GPIO4  | `#I2C0_SCL`   | -
I2C_DEV(0) | SDA    | GPIO5  | `#I2C0_SDA`   | -

</center><br>

## PWM Channels {#esp32_pwm_channels_esp32c3}

The ESP32-C3 LEDC module has 1 channel groups with 6 channels. Each of
these channels can be clocked by one of the 4 timers.

## SPI Interfaces {#esp32_spi_interfaces_esp32c3}

ESP32-C3 has three SPI controllers where SPI0 and SPI1 share the same bus.
They are used as interface for external memory and can only operate in memory
mode:

- Controller SPI0 is reserved for caching external memory like Flash
- Controller SPI1 is reserved for external memory like PSRAM
- Controller SPI2 can be used as general purpose SPI (also called HSPI)
- Controller SPI3 can be used as general purpose SPI (also called VSPI)

Thus, only SPI2 (FSPI) can be used as general purpose SPI in RIOT as
SPI_DEV(0).

The following table shows the pin configuration used for most boards, even
though it **can vary** from board to board.

<center>

Device                  | Signal | Pin    | Symbol      | Remarks
:-----------------------|:------:|:-------|:-----------:|:---------------------------
`SPI0_HOST`/`SPI1_HOST` | SPICS0 | GPIO14 | - | reserved for flash and PSRAM
`SPI0_HOST`/`SPI1_HOST` | SPICLK | GPIO15 | - | reserved for flash and PSRAM
`SPI0_HOST`/`SPI1_HOST` | SPID   | GPIO16 | - | reserved for flash and PSRAM
`SPI0_HOST`/`SPI1_HOST` | SPIQ   | GPIO17 | - | reserved for flash and PSRAM
`SPI0_HOST`/`SPI1_HOST` | SPIHD  | GPIO12 | - | reserved for flash and PSRAM (only in `qio` or `qout` mode)
`SPI0_HOST`/`SPI1_HOST` | SPIWP  | GPIO13 | - | reserved for flash and PSRAM (only in `qio` or `qout` mode)
`SPI2_HOST` (`FSPI`)    | SCK    | GPIO6  |`#SPI0_SCK`  | can be used
`SPI2_HOST` (`FSPI`)    | MOSI   | GPIO7  |`#SPI0_MOSI` | can be used
`SPI2_HOST` (`FSPI`)    | MISO   | GPIO2  |`#SPI0_MISO` | can be used
`SPI2_HOST` (`FSPI`)    | CS0    | GPIO10 |`#SPI0_CS0`  | can be used

</center><br>

## Timers {#esp32_timers_esp32c3}

ESP32-C3 has two timer groups with one timer each, resulting in a total of
two timers. Thus one timer with one channel can be used in RIOT
as timer device TIMER_DEV(0), because one timer is used as system timer.

ESP32-C3 do not have CCOMPARE registers. The counter implementation can not
be used.

## UART Interfaces {#esp32_uart_interfaces_esp32c3}

ESP32 integrates three UART interfaces. The following default pin
configuration of UART interfaces as used by a most boards can be overridden
by the application, see section [Application-Specific Configurations]
(#esp32_application_specific_configurations).

<center>

Device      |Signal|Pin     |Symbol      |Remarks
:-----------|:-----|:-------|:-----------|:----------------
UART_DEV(0) | TxD  | GPIO1  |`#UART0_TXD`| cannot be changed
UART_DEV(0) | RxD  | GPIO3  |`#UART0_RXD`| cannot be changed
UART_DEV(1) | TxD  | GPIO10 |`#UART1_TXD`| optional, can be overridden
UART_DEV(1) | RxD  | GPIO9  |`#UART1_RXD`| optional, can be overridden
UART_DEV(2) | TxD  | GPIO17 |`UART2_TXD` | optional, can be overridden
UART_DEV(2) | RxD  | GPIO16 |`UART2_RXD` | optional, can be overridden

</center><br>

## JTAG Interface {#esp32_jtag_interface_esp32c3}

There are two option on how to uese the JTAG interface on ESP32-C3:

1.  Using the built-in USB-to-JTAG bridge connected to an USB cable as follows:
    USB Signal     | ESP32-C3 Pin
    :--------------|:-----------
    D- (white)     | GPIO18
    D+ (green)     | GPIO19
    V_Bus (red)    | 5V
    Ground (black) | GND

2.  Using an external JTAG adapter connected to the JTAG interface exposed
    to GPIOs as follows:
    JTAG Signal | ESP32-C3 Pin
    :-----------|:-----------
    TRST_N      | CHIP_PU
    TDO         | GPIO7 (MTDO)
    TDI         | GPIO5 (MTDI)
    TCK         | GPIO6 (MTCK)
    TMS         | GPIO4 (MTMS)
    GND         | GND
    <br>
    @note This option requires that the USB D- and USB D+ signals are connected
          to the ESP32-C3 USB interface at GPIO18 and GPIO19.

Using the built-in USB-to-JTAG is the default option, i.e. the JTAG interface
of the ESP32-C3 is connected to the built-in USB-to-JTAG bridge. To use an
external JTAG adapter, the JTAG interface of the ESP32-C3 has to be connected
to the GPIOs as shown above. For this purpose eFuses have to be burned with
the following command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
espefuse.py burn_efuse JTAG_SEL_ENABLE --port /dev/ttyUSB0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once the eFuses are burned with this command and option `JTAG_SEL_ENABLE`,
GPIO10 is used as a bootstrapping pin to choose between the two options.
If GPIO10 is HIGH when ESP32-C3 is reset, the JTAG interface is connected
to the built-in USB to JTAG bridge and the USB cable can be used for on-chip
debugging. Otherwise, the JTAG interface is exposed to GPIO4 ... GPIO7
and an external JTAG adapter has to be used.

Alternatively, the integrated USB-to-JTAG bridge can be permanently disabled
with the following command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
espefuse.py burn_efuse DIS_USB_JTAG --port /dev/ttyUSB0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once the eFuses are burned with this command and option `DIS_USB_JTAG`,
the JTAG interface is always exposed to GPIO4 ... GPIO7 and an external
JTAG adapter has to be used.

@note Burning eFuses is an irreversible operation.

For more information about JTAG configuration for ESP32-C3, refer to the
section [Configure Other JTAG Interface]
(https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/jtag-debugging/configure-other-jtag.html)
in the ESP-IDF documentation.
