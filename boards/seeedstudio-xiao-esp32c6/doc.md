<!--
SPDX-FileCopyrightText: 2026 Stepan Konoplev
SPDX-License-Identifier: LGPL-2.1-only
-->

@defgroup    boards_seeedstudio-xiao-esp32c6 Seeed Studio Xiao ESP32C6
@ingroup     boards_esp32c6
@brief       Support for the Seeed Studio Xiao ESP32-C6 board
@author      Stepan Konoplev <stepan.konoplev@haw-hamburg.de>

\section seeedstudio-xiao-esp32c6 Seeed Studio Xiao ESP32C6

## Overview

Seeed Studio Xiao ESP32C6 is a small board based on the
Espressif ESP32-C6 SoC, featuring two 32-bit RISC-V processors: a
high-performance core running at up to 160 MHz and a low-power core
clocked up to 20 MHz.

- Wi-Fi 6 2.4 GHz
- Bluetooth 5 (LE)
- IEEE 802.15.4 (Zigbee, Thread)
- Memory: 512KB of SRAM, and 4MB of Flash
- Size: 21 x 17.8mm
- Battery charging chip: Supports lithium battery charge and discharge management
- Ultra-Low Power: Deep sleep power consumption is about 15μA
- Battery charge indicator red LED
- RF switch for built-in or external antenna
- Working temperature: -40°C ~ 85°C

<img src="https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32C6/img/xiaoc6.jpg" alt="Seeed Studio ESP32-C6 Xiao" width=200px />

## Hardware

### MCU

Most features of the board are provided by the ESP32-C6 SoC. For detailed
information about the ESP32-C6 variant (family) and ESP32x SoCs,
see section \ref esp32_mcu_esp32 "ESP32 SoC Series".

### Board Configuration

The Seeed Studio ESP32-C6 Xiao board has a reset button and a bootloader
button, as well as a user LED on GPIO15.
After reset, the bootloader button may be used by the application.

To select the board, add the following to the make command line:

```shell
make BOARD=seeedstudio-xiao-esp32c6
```

GPIO overview:

- 1 x UART
- 1 x LP_UART  (not supported)
- 1 x I2C
- 1 x LP_I2C (not supported)
- 1 x SPI
- 11 x GPIO(PWM only 3 defined by default)
- 7 x ADC (3 usable)
- 1 x SDIO (not supported)

The purpose for which a GPIO is used depends on which module
or function is used first. For example, if module `periph_spi` is not used,
the GPIOs listed in SPI configuration can be used for other purposes.

The following table shows the default board configuration.
This configuration can be overridden by \ref esp32_application_specific_configurations
"application-specific configurations".

<center>
Function        | Pin                                      | Configuration
:---------------|:-----------------------------------------|:-------------
BUTTON0(Boot)   | GPIO9                                    | GPIO9 is a [strapping pin](https://docs.espressif.com/projects/esptool/en/latest/esp32c6/advanced-topics/boot-mode-selection.html)
LED0            | GPIO15                                   | User LED (low active)
ADC/LP_GPIO     | GPIO0, GPIO1, GPIO2                      | \ref esp32_adc_channels "ADC Channels"
PWM_DEV(0)      | GPIO0, GPIO1, GPIO2                      | \ref esp32_pwm_channels "PWM Channels"
I2C_DEV(0):SDA  | GPIO22                                   | \ref esp32_i2c_interfaces "I2C Interfaces"
I2C_DEV(0):SCL  | GPIO23                                   | \ref esp32_i2c_interfaces "I2C Interfaces"
SPI_DEV(0):SCK  | GPIO19                                   | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MISO | GPIO20                                   | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):MOSI | GPIO18                                   | \ref esp32_spi_interfaces "SPI Interfaces"
SPI_DEV(0):CS0  | GPIO21                                   | \ref esp32_spi_interfaces "SPI Interfaces"
UART_DEV(0):TxD | GPIO16                                   | \ref esp32_uart_interfaces "UART interfaces"
UART_DEV(0):RxD | GPIO17                                   | \ref esp32_uart_interfaces "UART interfaces"
</center>
<br>

For detailed information about the peripheral configurations of ESP32-C6
boards, see section \ref esp32_peripherals "Common Peripherals".

### RF Antenna Switch

The board has an RF switch to toggle between the built-in
ceramic antenna and an external U.FL antenna. It is controlled by two GPIOs:

- GPIO3 (RF switch enable, active low) powers the switch.
- GPIO14 selects the antenna: low for the built-in ceramic antenna (default),
  high for the external U.FL antenna.

The built-in ceramic antenna is used by default, so no configuration is
required for the common case. The `board_init` function configures the RF
switch accordingly at startup.

To use the external U.FL antenna instead, enable the
`CONFIG_XIAO_ESP32C6_EXT_ANTENNA` option, for example by adding

​```
CFLAGS += -DCONFIG_XIAO_ESP32C6_EXT_ANTENNA=1
​```

to the application's Makefile.

@note The external antenna path has not been tested due to a missing external antenna.

### Battery Usage

The XIAO ESP32C6 series features a built-in power management chip, allowing it to be powered independently
by a battery or to charge the battery through its USB port.

For connecting a battery to your XIAO, it's recommend using a qualified rechargeable 3.7V lithium battery.
When soldering the battery, carefully distinguish between the positive and negative terminals.
The negative electrode pad should be located on the left side near the silk screen marking "D8,"
while the positive electrode pad should be located on the right side near the silk screen marking "D5."

@note When the board is powered from the battery, no voltage is present on
      the 5V pin.

@note The XIAO ESP32C6 has a red indicator light for battery charging, similar to the XIAO ESP32S3:

The red light behavior for the XIAO ESP32C6 is as follows:

- When no battery is connected:
  - The red light turns on when the Type-C cable is connected and turns off after 30 seconds.
- When a battery is connected and the Type-C cable is plugged in for charging:
  - The red light flashes.
- When the battery is fully charged via the Type-C connection:
  - The red light turns off.

To monitor the battery voltage on the XIAO ESP32C6, similar to the XIAO ESP32C3,
you'll need to solder a 200k resistor in a 1:2 configuration.
This setup reduces the voltage by half, allowing safe monitoring through the A0 analog port.

Sample Code can be found at [battery sample code](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/#sample-code)

### Deep Sleep and Wake-up

On the XIAO ESP32C6, only GPIO0, GPIO1 and GPIO2 of these RTC-capable pins are broken out
and can therefore be used as wake-up sources.

### Board Pinout

The following figure shows the pinout as configured by default board
definition.

<img src="https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32C6/img/XIAO_ESP32-C6_front_pinout.png" alt="Seeed Studio ESP32-C6 Xiao Pinout Front" width=800 />
<img src="https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32C6/img/XIAO_ESP32-C6_back_pinout.png" alt="Seeed Studio ESP32-C6 Xiao Pinout Back" width=800 />

### Board documentation

- [schematic](https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32C6/XIAO_ESP32_C6_v1.0_SCH_260114.pdf) (PDF)
- [pinout](https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32C6/res/XIAO_ESP32C6_Pinout.xlsx) (XLSX)
- [ESP32-C6 datasheet](https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32C6/res/esp32-c6_datasheet_en.pdf) (PDF)
- [ESP32-C6 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-c6_technical_reference_manual_en.pdf) (PDF)
- [product page](https://www.seeedstudio.com/Seeed-Studio-XIAO-ESP32C6-p-5884.html)
- [wiki page](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)

## Flashing the Device

The USB-C connector of the board is directly connected to the USB Serial/JTAG
interface of the ESP32-C6 SoC. It can be used to program the board and to debug
the application. Just connect the board to your host computer and use the
following command:

```shell
make BOARD=seeedstudio-xiao-esp32c6 flash
```
