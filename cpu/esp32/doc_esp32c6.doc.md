<!--
SPDX-FileCopyrightText: 2025 Gunar Schorcht
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup   cpu_esp32_esp32c6 ESP32-C6 family
@ingroup    cpu_esp32
@brief      Specific properties of ESP32-C6 variant (family)
@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32_riot_esp32c6  Specific properties of ESP32-C6 variant (family)

## GPIO pins {#esp32_gpio_pins_esp32c6}

ESP32-C6 has 19 broken-out GPIO pins, where a subset can be used as ADC channel
and as low-power digital inputs/outputs in deep-sleep mode, the so-called
LP GPIOs. Some of them are used by special SoC components. The following
table gives a short overview.

<center>

Pin    | Type   | ADC / LP | PU / PD | Special function | Remarks
-------|:-------|:--------:|:-------:|------------------|--------
GPIO0  | In/Out | ADC / LP | yes     | XTAL_32K_P       | -
GPIO1  | In/Out | ADC / LP | yes     | XTAL_32K_N       | -
GPIO2  | In/Out | ADC / LP | yes     |                  | FSPIQ
GPIO3  | In/Out | ADC / LP | yes     |                  | -
GPIO4  | In/Out | ADC / LP | yes     | MTMS             | FSPIWP
GPIO5  | In/Out | ADC / LP | yes     | MTDI             | FSPIHD
GPIO6  | In/Out | ADC / LP | yes     | MTCK             | FSPICLK
GPIO7  | In/Out | LP       | yes     | MTDO             | FSPID
GPIO8  | In/Out |          | yes     |                  | Bootstrapping
GPIO9  | In/Out |          | yes     |                  | Bootstrapping, pulled up
GPIO10 | In/Out |          | yes     |                  | N/A on chips with in-package flash
GPIO11 | In/Out |          | yes     |                  | N/A on chips with in-package flash
GPIO12 | In/Out |          | yes     | USB D-           | USB Serial / JTAG interface
GPIO13 | In/Out |          | yes     | USB D+           | USB Serial / JTAG interface
GPIO14 | In/Out |          | yes     |                  | N/A on chips w/o in-package flash
GPIO15 | In/Out |          | yes     |                  | Bootstrapping
GPIO16 | In/Out |          | yes     | UART0 TX         | FSPICS0
GPIO17 | In/Out |          | yes     | UART0 RX         | FSPICS1
GPIO18 | In/Out |          | yes     | UART0 RX         | FSPICS2
GPIO19 | In/Out |          | yes     |                  | FSPICS3
GPIO20 | In/Out |          | yes     |                  | FSPICS4
GPIO21 | In/Out |          | yes     |                  | FSPICS5
GPIO22 | In/Out |          | yes     |                  | -
GPIO23 | In/Out |          | yes     |                  | -
GPIO24 | In/Out |          | yes     | SPICS0           | N/A on chips with in-package flash
GPIO25 | In/Out |          | yes     | SPIQ             | N/A on chips with in-package flash
GPIO26 | In/Out |          | yes     | SPIWP            | N/A on chips with in-package flash
GPIO27 | In/Out |          | yes     | VDD_SPI          | N/A on chips with in-package flash
GPIO28 | In/Out |          | yes     | SPIHD            | N/A on chips with in-package flash
GPIO29 | In/Out |          | yes     | SPICLK           | N/A on chips with in-package flash
GPIO30 | In/Out |          | yes     | SPID             | N/A on chips with in-package flash

</center><br>

<b>ADC:</b> these pins can be used as ADC inputs<br>
<b>LP:</b> these pins are LP GPIOs and can be used in deep-sleep mode<br>
<b>PU/PD:</b> these pins have software configurable pull-up/pull-down functionality.<br>

GPIO8 and GPIO9 are bootstrapping pins which are used to boot
ESP32-C6 in different modes:

<center>

GPIO9 | GPIO8 | Mode
:----:|:-----:|:------------------------------------------------------------
1     | X     | SPI Boot mode to boot the firmware from flash (default mode)
0     | 1     | SPI Download Boot mode

Other combinations are invalid.

</center><br>

## ADC Channels {#esp32_adc_channels_esp32c6}

ESP32-C6 integrates one 12-bit ADC with 7 channels in
total: GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5 and GPIO6

The maximum number of ADC channels #ADC_NUMOF_MAX is 7.

## I2C Interfaces {#esp32_i2c_interfaces_esp32c6}

ESP32-C6 has two built-in I2C interfaces.

The following table shows the default configuration of I2C interfaces
used for ESP32-C6 boards. It can be overridden by
[application-specific configurations](#esp32_application_specific_configurations).

<center>

Device     | Signal | Pin    | Symbol        | Remarks
:----------|:-------|:-------|:--------------|:----------------
I2C_DEV(0) |        |        | `I2C0_SPEED`  | default is `I2C_SPEED_FAST`
I2C_DEV(0) | SCL    | GPIO5  | `I2C0_SCL`    | -
I2C_DEV(0) | SDA    | GPIO4  | `I2C0_SDA`    | -

</center><br>

## PWM Channels {#esp32_pwm_channels_esp32c6}

The ESP32-C6 LEDC module has 1 channel group with 6 channels. Each of
these channels can be clocked by one of the 4 timers. This means that
it is possible to define up to 4 PWM devices with different frequencies
and resolutions and a total of 6 PWM channels.

## SPI Interfaces {#esp32_spi_interfaces_esp32c6}

ESP32-C6 has three SPI controllers where SPI0 and SPI1 share the same bus.
They are used as interface for external memory and can only operate in memory
mode:

- Controller SPI0 is reserved for caching external memory like Flash
- Controller SPI1 is reserved for external memory like PSRAM
- Controller SPI2 can be used as general purpose SPI (also called FSPI)

Thus, only SPI2 (FSPI) can be used as general purpose SPI in RIOT as
SPI_DEV(0).

The following table shows the pin configuration used for most boards, even
though it **can vary** from board to board.

<center>

Device        | Signal | Pin    | Symbol      | Remarks
:-------------|:------:|:-------|:-----------:|:---------------------------
SPI_DEV(0)    | SCK    | GPIO6  |`SPI0_SCK`   | `SPI2_HOST` (`FSPI`)
SPI_DEV(0)    | MOSI   | GPIO7  |`SPI0_MOSI`  | `SPI2_HOST` (`FSPI`)
SPI_DEV(0)    | MISO   | GPIO2  |`SPI0_MISO`  | `SPI2_HOST` (`FSPI`)
SPI_DEV(0)    | CS0    | GPIO18 |`SPI0_CS0`   | `SPI2_HOST` (`FSPI`)

</center><br>

## Timers {#esp32_timers_esp32c6}

ESP32-C6 has two timer groups with one timer each, resulting in a total of
two timers. Thus one timer with one channel can be used in RIOT
as timer device TIMER_DEV(0), because the other timer is used as system timer.

ESP32-C6 do not have CCOMPARE registers. The counter implementation can not
be used.

## UART Interfaces {#esp32_uart_interfaces_esp32c6}

ESP32-C6 integrates three UART interfaces, two in the high-power domain and
one in the low-power domain. The low-power UART interface is not yet supported.
The following default pin configuration of UART interfaces as used by a most
boards can be overridden by the application, see section
[Application-Specific Configurations](#esp32_application_specific_configurations).

<center>

Device      |Signal|Pin     |Symbol      |Remarks
:-----------|:-----|:-------|:-----------|:----------------
UART_DEV(0) | TxD  | GPIO16 |`UART0_TXD` | cannot be changed
UART_DEV(0) | RxD  | GPIO17 |`UART0_RXD` | cannot be changed
UART_DEV(1) | TxD  |        |`UART1_TXD` | optional, can be configured
UART_DEV(1) | RxD  |        |`UART1_RXD` | optional, can be configured

</center><br>

## JTAG Interface {#esp32_jtag_interface_esp32c6}

There are two options on how to use the JTAG interface on ESP32-C6:

1.  Using the built-in USB-to-JTAG bridge connected to an USB cable as follows:
    <center>
    USB Signal     | ESP32-C6 Pin
    :--------------|:-----------
    D- (white)     | GPIO12
    D+ (green)     | GPIO13
    V_Bus (red)    | 5V
    Ground (black) | GND
    </center><br>

2.  Using an external JTAG adapter connected to the JTAG interface exposed
    to GPIOs as follows:
    <center>
    JTAG Signal | ESP32-C6 Pin
    :-----------|:-----------
    TRST_N      | CHIP_PU
    TDO         | GPIO7 (MTDO)
    TDI         | GPIO5 (MTDI)
    TCK         | GPIO6 (MTCK)
    TMS         | GPIO4 (MTMS)
    GND         | GND
    </center><br>

Using the built-in USB-to-JTAG bridge is the default option, i.e. the JTAG
interface of the ESP32-C6 is connected to the built-in USB-to-JTAG bridge.
To use an external JTAG adapter, the JTAG interface of the ESP32-C6 has to
be connected to the GPIOs as shown above. For this purpose eFuses have to be
burned with the following command:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
espefuse.py burn_efuse JTAG_SEL_ENABLE --port /dev/ttyUSB0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
@warning Burning eFuses is an irreversible operation.

Once the eFuses are burned with this command and option `JTAG_SEL_ENABLE`,
GPIO15 is used as a bootstrapping pin to choose between the two options.
If GPIO15 is HIGH when ESP32-C6 is reset, the JTAG interface is connected
to the built-in USB to JTAG bridge and the USB cable can be used for on-chip
debugging. Otherwise, the JTAG interface is exposed to GPIO4 ... GPIO7
and an external JTAG adapter has to be used.

Alternatively, the integrated USB-to-JTAG bridge can be permanently disabled
with the following command:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
espefuse.py burn_efuse DIS_USB_JTAG --port /dev/ttyUSB0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
@warning Burning eFuses is an irreversible operation.

Once the eFuses are burned with this command and option `DIS_USB_JTAG`,
the JTAG interface is always exposed to GPIO4 ... GPIO7 and an external
JTAG adapter has to be used.

For more information about JTAG configuration for ESP32-C6, refer to the
section [Configure Other JTAG Interface]
(https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-guides/jtag-debugging/configure-other-jtag.html)
in the ESP-IDF documentation.
