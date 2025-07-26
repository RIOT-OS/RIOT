<!--
Copyright (C) 2025 Gunar Schorcht

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
-->

@defgroup     stdio_usb_serial_jtag  STDIO over ESP32 Debug Serial/JTAG
@ingroup      sys_stdio
@brief        STDIO via the USB Serial/JTAG debug interface found on some ESP32 SoCs

Some members of the ESP32 family (ESP32-C3, ESP32-H2, ESP32-S3) provide a on-chip
debug interface that provides a serial console and JTAG via USB.
To route STDIO to this debug console, enable this module.

    USEMODULE += stdio_usb_serial_jtag

@see          cpu_esp32

@defgroup   cpu_esp32 ESP32 SoC Series
@ingroup    cpu
@brief      Implementation for Espressif ESP32 SoC Series

This document describes the RIOT implementation for supported variants
(families) of Espressif's ESP32 SoC series.

@author     Gunar Schorcht <gunar@schorcht.net>

\section esp32_riot  RIOT-OS on ESP32 SoC Series Boards

## Table of Contents {#esp32_toc}

1. [Overview](#esp32_overview)
2. [Short Configuration Reference](#esp32_short_configuration_reference)
3. [ESP32x MCU](#esp32_mcu_esp32)
    1. [Features of ESP32x SoCs](#esp32_features)
    2. [Features Supported by RIOT-OS](#esp32_supported_features)
    3. [Limitations of the RIOT-port](#esp32_limitations)
4. [Toolchain](#esp32_toolchain)
    1. [RIOT Docker Toolchain (riotdocker)](#esp32_riot_docker_toolchain)
    2. [Local Toolchain Installation](#esp32_local_toolchain_installation)
5. [Flashing the Device](#esp32_flashing_the_device)
    1. [Toolchain Usage](#esp32_toolchain_usage)
    2. [Compile Options](#esp32_compile_options)
    3. [Flash Modes](#esp32_flash_modes)
    4. [ESP-IDF Heap Implementation](#esp32_esp_idf_heap_implementation)
6. [Common Peripherals](#esp32_peripherals)
    1. [GPIO pins](#esp32_gpio_pins)
    2. [ADC Channels](#esp32_adc_channels)
    3. [DAC Channels](#esp32_dac_channels)
    4. [I2C Interfaces](#esp32_i2c_interfaces)
    5. [PWM Channels](#esp32_pwm_channels)
    6. [SDMMC Interfaces](#esp32_sdmmc_interfaces)
    8. [SPI Interfaces](#esp32_spi_interfaces)
    9. [Timers](#esp32_timers)
    10. [RTT Implementation](#esp32_rtt_counter)
    12. [UART Interfaces](#esp32_uart_interfaces)
    13. [CAN Interfaces](#esp32_can_interfaces)
    14. [Power Management](#esp32_power_management)
    15. [Other Peripherals](#esp32_other_peripherals)
7. [Special On-board Peripherals](#esp32_special_on_board_peripherals)
    1. [SPI RAM Modules](#esp32_spi_ram)
    2. [SPIFFS Device](#esp32_spiffs_device)
8. [Network Interfaces](#esp32_network_interfaces)
    1. [Ethernet MAC Network Interface](#esp32_ethernet_network_interface)
    2. [WiFi Network Interface](#esp32_wifi_network_interface)
    3. [WiFi SoftAP Network Interface](#esp32_wifi_ap_network_interface)
    4. [ESP-NOW Network Interface](#esp32_esp_now_network_interface)
    5. [Bluetooth Interface](#esp32_esp_bluetooth_interface)
    6. [IEEE 802.15.4 Network Interface](#esp32_esp_ieee802154_interface)
    7. [Other Network Devices](#esp32_other_network_devices)
10. [Application-Specific Configurations](#esp32_application_specific_configurations)
    1. [Make Variable `CFLAGS`](#esp32_config_make_variable)
    2. [Application-Specific Board Configuration](#esp32_application_specific_board_configuration)
    3. [Application-Specific Driver Configuration](#esp32_application_specific_driver_configuration)
11. [Debugging](#esp32_debugging)
    1. [JTAG Debugging](#esp32_jtag_debugging)
    2. [QEMU Mode and GDB](#esp32_qemu_mode_and_gdb)


# Overview {#esp32_overview}

The **RIOT port for ESP32** is an implementation of **RIOT-OS** for
the **Espressif ESP32 SoC series** (hereafter called ESP32x), which supports
most of the functions of RIOT-OS.

Due to the potential confusion caused by referring to the SoC series in
exactly the same way as the namesaking SoC variant, we need to define a
terminology in this document to distinguish between a variant and a set
of variants:

- **ESP32 SoC Series** denotes the set of all supported SoC variants (families).
- **ESP32x SoC** or short **ESP32x** denotes any or a couple of SoC variants (families).
- **EPS32 SoC** or short **ESP32** denotes the ESP32 SoC variant (family).

@note Variants of Espressif's ESP32 SoC Series are called ESP32x families
      in RIOT's terminology.

[Back to table of contents](#esp32_toc)

# Short Configuration Reference {#esp32_short_configuration_reference}

The following table gives a short reference of all board configuration
parameters used by the RIOT port for ESP32x SoCs in alphabetical order.

<center>

Parameter                               | Short Description                                                 | Type[1]
----------------------------------------|-------------------------------------------------------------------|------
[ADC_GPIOS](#esp32_adc_channels)        | GPIOs that can be used as ADC channels                            | m
[CAN_TX](#esp32_can_interfaces)         | GPIO used as CAN transceiver TX signal                            | o
[CAN_RX](#esp32_can_interfaces)         | GPIO used as CAN transceiver RX signal                            | o
[DAC_GPIOS](#esp32_dac_channels)        | GPIOs that can be used as DAC channels                            | m
[I2C0_SPEED](#esp32_i2c_interfaces)     | Bus speed of I2C_DEV(0)                                           | o
[I2C0_SCL](#esp32_i2c_interfaces)       | GPIO used as SCL for I2C_DEV(0)                                   | o
[I2C0_SDA](#esp32_i2c_interfaces)       | GPIO used as SCL for I2C_DEV(0                                    | o
[I2C1_SPEED](#esp32_i2c_interfaces)     | Bus speed of I2C_DEV(1)                                           | o
[I2C1_SCL](#esp32_i2c_interfaces)       | GPIO used as SCL for I2C_DEV(1)                                   | o
[I2C1_SDA](#esp32_i2c_interfaces)       | GPIO used as SCL for I2C_DEV(1)                                   | o
[PWM0_GPIOS](#esp32_pwm_channels)       | GPIOs that can be used at channels of PWM_DEV(0)                  | o
[PWM1_GPIOS](#esp32_pwm_channels)       | GPIOs that can be used at channels of PWM_DEV(1)                  | o
[PWM3_GPIOS](#esp32_pwm_channels)       | GPIOs that can be used at channels of PWM_DEV(2)                  | o
[PWM4_GPIOS](#esp32_pwm_channels)       | GPIOs that can be used at channels of PWM_DEV(3)                  | o
[SPI0_CTRL](#esp32_spi_interfaces)      | SPI Controller used for SPI_DEV(0), can be `VSPI` `HSPI`          | o
[SPI0_SCK](#esp32_spi_interfaces)       | GPIO used as SCK for SPI_DEV(0)                                   | o
[SPI0_MOSI](#esp32_spi_interfaces)      | GPIO used as MOSI for SPI_DEV(0)                                  | o
[SPI0_MISO](#esp32_spi_interfaces)      | GPIO used as MISO for SPI_DEV(0)                                  | o
[SPI0_CS0](#esp32_spi_interfaces)       | GPIO used as default CS for SPI_DEV(0)                            | o
[SPI1_CTRL](#esp32_spi_interfaces)      | SPI Controller used for SPI_DEV(1), can be `VSPI` `HSPI`          | o
[SPI1_SCK](#esp32_spi_interfaces)       | GPIO used as SCK for SPI_DEV(1)                                   | o
[SPI1_MOSI](#esp32_spi_interfaces)      | GPIO used as MOSI for SPI_DEV(1)                                  | o
[SPI1_MISO](#esp32_spi_interfaces)      | GPIO used as MISO for SPI_DEV(1)                                  | o
[SPI1_CS0](#esp32_spi_interfaces)       | GPIO used as default CS for SPI_DEV(1)                            | o
[UART1_TXD](#esp32_uart_interfaces)     | GPIO used as TxD for UART_DEV(1)                                  | o
[UART1_RXD](#esp32_uart_interfaces)     | GPIO used as RxD for UART_DEV(1)                                  | o
[UART2_TXD](#esp32_uart_interfaces)     | GPIO used as TxD for UART_DEV(2)                                  | o
[UART2_RXD](#esp32_uart_interfaces)     | GPIO used as RxD for UART_DEV(2)                                  | o

</center>

1. **Type**: m - mandatory, o - optional

The following table gives a short reference  in alphabetical order of modules
that can be enabled/disabled by board configurations and/or application's
makefile using `USEMODULE` and `DISABLE_MODULE`.

<center>

Module                                              | Default   | Short description
----------------------------------------------------|-----------|-------------------
[esp_eth](#esp32_ethernet_network_interface)        | not used  | enable the Ethernet MAC (EMAC) network device
[esp_gdb](#esp32_debugging)                         | not used  | enable the compilation with debug information for debugging
[esp_hw_counter](#esp32_timers)                     | not used  | use hardware counters for RIOT timers
[esp_i2c_hw](#esp32_i2c_interfaces)                 | not used  | use the i2C hardware implementation
[esp_idf_heap](#esp32_esp_idf_heap_implementation)  | not used  | enable ESP-IDF heap implementation
[esp_log_colored](#esp32_esp_log_module)            | not used  | enable colored log output
[esp_log_startup](#esp32_esp_log_module)            | not used  | enable additional startup information
[esp_log_tagged](#esp32_esp_log_module)             | not used  | add additional information to the log output
[esp_now](#esp32_esp_now_network_interface)         | not used  | enable the ESP-NOW network device
[esp_qemu](#esp32_esp_qemu)                         | not used  | build QEMU for ESP32 application image
[esp_spi_oct](#esp32_spi_ram)                       | not used  | enable SPI RAM in Octal SPI Mode
[esp_rtc_timer_32k](#esp32_rtt_counter)             | not used  | use RTC timer with external 32.768 kHz crystal as RTT
[esp_spi_ram](#esp32_spi_ram)                       | not used  | enable SPI RAM
[esp_spiffs](#esp32_spiffs_device)                  | not used  | enable SPIFFS for on-board flash memory
[esp_wifi](#esp32_wifi_network_interface)           | not used  | enable the Wifi network device in WPA2 personal mode
[esp_wifi_ap](#esp32_wifi_ap_network_interface)     | not used  | enable the WiFi SoftAP network device
[esp_wifi_enterprise](#esp32_wifi_network_interface)| not used  | enable the Wifi network device in WPA2 enterprise mode

</center><br>

[Back to table of contents](#esp32_toc)

# ESP32x MCU {#esp32_mcu_esp32}

ESP32x SoCs are low-cost, ultra-low-power, single or dual-core SoCs from
Espressif Systems with integrated WiFi and Bluetooth BLE module. The SoCs are
either based on

- Tensilica Xtensa 32-bit LX6 microprocessor (ESP32),
- Tensilica Xtensa 32-bit LX7 microprocessor (ESP32-S2, ESP32-S3), or
- 32-bit RISC-V CPU (ESP32-C3, ESP32-C3 ESP32-H2).

At the moment, ESP32, ESP32-S2, ESP32-S3, ESP32-C3 and ESP32-H2
variants (families) are supported by RIOT-OS.

@note Even if the used ESP32x SoC is a dual-core version, RIOT-OS uses only
      one core.

[Back to table of contents](#esp32_toc)

## Features of ESP32x SoCs {#esp32_features}

The ESP32 SoC Series consists of different ESP32x SoC variants (families),
which differ in the type and the number of processor cores used and the
hardware modules supported.

### Features of The ESP32 SoC variant (family)

The key features of ESP32 are:

<center>

| MCU               | ESP32                                                             | Supported by RIOT |
| ------------------|-------------------------------------------------------------------|------------------ |
| Vendor            | Espressif                                                         | |
| Cores             | 1 or 2 x Tensilica Xtensa LX6                                     | 1 core |
| FPU               | ULP - Ultra low power co-processor                                | no |
| RAM               | 520 KiB SRAM <br> 8 KiB slow RTC SRAM <br> 8 KiB fast RTC SRAM    | yes <br> yes <br> yes |
| ROM               | 448 KiB                                                           | yes |
| Flash             | 512 KiB ... 16 MiB                                                | yes |
| Frequency         | 240 MHz, 160 MHz, 80 MHz                                          | yes |
| Power Consumption | 68 mA @ 240 MHz <br> 44 mA @ 160 MHz (34 mA @ 160 MHz single core) <br> 31 mA @ 80 MHz (25 mA @ 80 MHz single core) <br> 800 uA in light sleep mode <br> 10 uA in deep sleep mode | yes <br> yes <br> yes <br> yes <br> yes |
| Timer             | 4 x 64 bit                                                        | yes |
| ADC               | 2 x SAR-ADC with up to 18 x 12 bit channels total                 | yes |
| DAC               | 2 x DAC with 8 bit                                                | yes |
| GPIO              | 34 (6 are only inputs, 18 are RTC GPIOs)                          | yes |
| I2C               | 2                                                                 | yes |
| SDMMC             | 2                                                                 | yes |
| SPI               | 4 (2 are usable as general purpose SPI)                           | yes |
| UART              | 3                                                                 | yes |
| WiFi              | IEEE 802.11 b/g/n built in                                        | yes |
| Bluetooth         | v4.2 BR/EDR and BLE                                               | yes |
| Ethernet          | MAC interface with dedicated DMA and IEEE 1588 support            | yes |
| CAN               | version 2.0                                                       | yes |
| IR                | up to 8 channels TX/RX                                            | no |
| Motor PWM         | 2 devices x 6 channels                                            | no |
| LED PWM           | 16 channels with 20 bit resolution in 2 channel groups with 4 timers | yes |
| Crypto            | Hardware acceleration of AES, SHA-2, RSA, ECC, RNG                | no |
| Vcc               | 2.5 - 3.6 V                                                       | |
| Documents         | [Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf) <br> [Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf) | |

</center><br>

### Features of The ESP32-C3 SoC variant (family)

The key features of ESP32-C3 are:

<center>

| MCU               | ESP32-C3                                                          | Supported by RIOT |
| ------------------|-------------------------------------------------------------------|------------------ |
| Vendor            | Espressif                                                         | |
| Cores             | 1 x 32-bit RISC-V core                                            | yes |
| FPU               | -                                                                 | -   |
| RAM               | 400 KiB SRAM <br> 8 KiB RTC SRAM                                  | yes <br> yes |
| ROM               | 384 KiB                                                           | yes |
| Flash             | 8 MiB                                                             | yes |
| Frequency         | 160 MHz, 80 MHz                                                   | yes |
| Power Consumption | 20 mA @ 240 MHz <br> 15 mA @ 80 MHz <br> 130 uA in light sleep mode <br> 5 uA in deep sleep mode | yes <br> yes <br> yes <br> yes |
| Timer             | 2 x 54 bit                                                        | yes |
| ADC               | 2 x SAR-ADC with up to 6 x 12 bit channels total                  | yes |
| DAC               | -                                                                 | -   |
| GPIO              | 22                                                                | yes |
| I2C               | 1                                                                 | yes |
| SPI               | 3 (1 is usable as general purpose SPI)                            | yes |
| UART              | 2                                                                 | yes |
| WiFi              | IEEE 802.11 b/g/n built in                                        | yes |
| Bluetooth         | Bluetooth 5 (LE)                                                  | yes |
| Ethernet          | -                                                                 | -   |
| CAN               | version 2.0                                                       | yes |
| IR                | up to 4 channels TX/RX                                            | -   |
| Motor PWM         | -                                                                 | no  |
| LED PWM           | 6 channels with 14 bit resolution in 1 channel group with 4 timers | yes |
| Crypto            | Hardware acceleration of AES, SHA-2, RSA, ECC, RNG                | no  |
| Vcc               | 3.0 - 3.6 V                                                       | |
| Documents         | [Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf) <br> [Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf) | |

</center><br>

### Features of The ESP32-H2 SoC variant (family)

The key features of ESP32-H2 are:

<center>

| MCU               | ESP32-H2                                                          | Supported by RIOT |
| ------------------|-------------------------------------------------------------------|------------------ |
| Vendor            | Espressif                                                         | |
| Cores             | 1 x 32-bit RISC-V core                                            | yes |
| FPU               | -                                                                 | -   |
| RAM               | 320 KiB SRAM <br> 4 KiB LP SRAM                                   | yes <br> yes |
| ROM               | 384 KiB                                                           | yes |
| Flash             | 4 MiB                                                             | yes |
| Frequency         | 96 MHz, 64 Mhz, 48 MHz                                            | yes |
| Power Consumption | 17 mA @ 96 MHz <br> 13 mA @ 64 MHz <br> 11 mA @ 64 MHz <br> 85 uA in light sleep mode <br> 7 uA in deep sleep mode | yes <br> yes <br> yes <br> yes <br> yes |
| Timer             | 2 x 54 bit                                                        | yes |
| ADC               | 1 x SAR-ADC with up to 5 x 12 bit channels total                  | yes |
| DAC               | -                                                                 | -   |
| GPIO              | 28                                                                | yes |
| I2C               | 2                                                                 | yes |
| SPI               | 2 (1 is usable as general purpose SPI)                            | yes |
| UART              | 2                                                                 | yes |
| WiFi              | -                                                                 | -   |
| Bluetooth         | Bluetooth 5.3 (LE)                                                | yes |
| IEEE 802.15.4     | 250 Kbps data rate in 2.4 GHz band with OQPSK PHY                 | yes |
| Ethernet          | -                                                                 | -   |
| CAN               | version 2.0                                                       | yes |
| IR                | up to 4 channels TX/RX                                            | -   |
| Motor PWM         | -                                                                 | no  |
| LED PWM           | 6 channels with 14 bit resolution in 1 channel group with 4 timers | yes |
| Crypto            | Hardware acceleration of AES, SHA-2, RSA, ECC, RNG                | no  |
| Vcc               | 3.0 - 3.6 V                                                       | |
| Documents         | [Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-h2_datasheet_en.pdf) <br> [Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-h2_technical_reference_manual_en.pdf) | |

</center><br>

### Features of The ESP32-S2 SoC variant (family)

The key features of ESP32-S2 are:

<center>

| MCU               | ESP32-S2                                                          | Supported by RIOT |
| ------------------|-------------------------------------------------------------------|------------------ |
| Vendor            | Espressif                                                         | |
| Cores             | 1 x Tensilica Xtensa LX7                                          | 1 core |
| FPU               | ULP - Ultra low power co-processor                                | no |
| RAM               | 320 KiB SRAM <br> 8 KiB slow RTC SRAM <br> 8 KiB fast RTC SRAM    | yes <br> yes <br> yes |
| ROM               | 128 KiB                                                           | yes |
| Flash             | 512 KiB ... 32 MiB Dual/Quad/Octal SPI (external or internal)     | yes |
| Frequency         | 240 MHz, 160 MHz, 80 MHz                                          | yes |
| Power Consumption | 66 mA @ 240 MHz <br> 50 mA @ 160 MHz (40 mA @ 160 MHz single core) <br> 33 mA @ 80 MHz (28 mA @ 80 MHz single core) <br> 19 mA @ 40 MHz (16 mA @ 40 MHz single core) <br>240 uA in light sleep mode <br> 8 uA in deep sleep mode | yes <br> yes <br> yes <br> yes <br> yes <br> yes |
| Timer             | 4 x 54 bit                                                        | yes |
| ADC               | 2 x SAR-ADC with up to 20 x 13 bit channels total                 | yes |
| DAC               | 2 x DAC with 8 bit                                                | -   |
| GPIO              | 43 (22 are RTC GPIOs)                                             | yes |
| I2C               | 2                                                                 | yes |
| SPI               | 4 (2 are usable as general purpose SPI)                           | yes |
| UART              | 2                                                                 | yes |
| WiFi              | IEEE 802.11 b/g/n built in                                        | yes |
| Bluetooth         | -                                                                 | -   |
| Ethernet          | -                                                                 | -   |
| CAN               | version 2.0                                                       | yes |
| IR                | up to 4 channels TX/RX                                            | no |
| Motor PWM         | 2 devices x 6 channels                                            | no |
| LED PWM           | 8 channels with 14 bit resolution in 1 channel group with 4 timers | yes |
| Crypto            | Hardware acceleration of AES, SHA-2, RSA, ECC, RNG                | no |
| Vcc               | 2.8 - 3.6 V                                                       | |
| Documents         | [Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_en.pdf) <br> [Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf) | |

</center><br>

### Features of The ESP32-S3 SoC variant (family)

The key features of ESP32-S3 are:

<center>

| MCU               | ESP32-S3                                                          | Supported by RIOT |
| ------------------|-------------------------------------------------------------------|------------------ |
| Vendor            | Espressif                                                         | |
| Cores             | 2 x Tensilica Xtensa LX7                                          | 1 core |
| FPU               | ULP - Ultra low power co-processor                                | no |
| RAM               | 512 KiB SRAM <br> 8 KiB slow RTC SRAM <br> 8 KiB fast RTC SRAM    | yes <br> yes <br> yes |
| ROM               | 384 KiB                                                           | yes |
| Flash             | 512 KiB ... 32 MiB Dual/Quad/Octal SPI (external or internal)     | yes |
| Frequency         | 240 MHz, 160 MHz, 80 MHz                                          | yes |
| Power Consumption | 66 mA @ 240 MHz <br> 50 mA @ 160 MHz (40 mA @ 160 MHz single core) <br> 33 mA @ 80 MHz (28 mA @ 80 MHz single core) <br> 19 mA @ 40 MHz (16 mA @ 40 MHz single core) <br>240 uA in light sleep mode <br> 8 uA in deep sleep mode | yes <br> yes <br> yes <br> yes <br> yes <br> yes |
| Timer             | 4 x 54 bit                                                        | yes |
| ADC               | 2 x SAR-ADC with up to 20 x 12 bit channels total                 | yes |
| DAC               | -                                                                 | -   |
| GPIO              | 45 (22 are RTC GPIOs)                                             | yes |
| I2C               | 2                                                                 | yes |
| SDMMC             | 2                                                                 | yes |
| SPI               | 4 (2 are usable as general purpose SPI)                           | yes |
| UART              | 3                                                                 | yes |
| WiFi              | IEEE 802.11 b/g/n built in                                        | yes |
| Bluetooth         | Bluetooth 5 (LE)                                                  | yes |
| Ethernet          | -                                                                 | -   |
| CAN               | version 2.0                                                       | yes |
| IR                | up to 8 channels TX/RX                                            | no |
| Motor PWM         | 2 devices x 6 channels                                            | no |
| LED PWM           | 8 channels with 14 bit resolution in 1 channel group with 4 timers | yes |
| Crypto            | Hardware acceleration of AES, SHA-2, RSA, ECC, RNG                | no |
| Vcc               | 2.3 - 3.6 V                                                       | |
| Documents         | [Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf) <br> [Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf) | |

</center><br>

Rather than using the ESP32x SoCs directly, ESP32x boards use an
[ESP32x module from Espressif](https://www.espressif.com/en/products/hardware/modules) which
integrates additionally to the SoC some key components, like SPI flash memory, SPI RAM, or crystal
oscillator. Some of these components are optional.

Most common modules used by ESP32x SoC boards are:

- [ESP32-WROOM-32](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf)
- [ESP32-WROVER](https://www.espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf)
- [ESP32-C3-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-c3-mini-1_datasheet_en.pdf)
- [ESP32-C3-WROOM-02](https://www.espressif.com/sites/default/files/documentation/esp32-c3-wroom-02_datasheet_en.pdf)
- [ESP32-H2-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-h2-mini-1_mini-1u_datasheet_en.pdf)
- [ESP32-S2-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-s2-mini-1_esp32-s2-mini-1u_datasheet_en.pdf)
- [ESP32-S2-SOLO](https://www.espressif.com/sites/default/files/documentation/esp32-s2-solo_esp32-s2-solo-u_datasheet_en.pdf)
- [ESP32-S2-WROOM](https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_en.pdf)
- [ESP32-S2-WROVER](https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_en.pdf)
- [ESP32-S3-MINI-1](https://www.espressif.com/sites/default/files/documentation/esp32-s3-mini-1_mini-1u_datasheet_en.pdf)
- [ESP32-S3-WROOM-1](https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf)
- [ESP32-S3-WROOM-2](https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-2_datasheet_en.pdf)

[Back to table of contents](#esp32_toc)

## Features Supported by RIOT-OS {#esp32_supported_features}

The RIOT-OS for ESP32x SoCs supports the following features at the moment:

- I2C interfaces
- SPI interfaces
- UART interfaces
- CAN interface
- CPU ID access
- RTT module
- ADC and DAC channels
- PWM channels
- SPI RAM
- SPI Flash Drive (MTD with SPIFFS and VFS)
- Layered Power Management
- Hardware number generator
- Hardware timer devices
- ESP-NOW netdev interface
- ESP WiFi netdev interface in (WPA2 Personal Mode/Enterprise Mode, SoftAP mode)
- ESP Ethernet MAC (EMAC) netdev interface
- IEEE 802.15.4 HAL interface

[Back to table of contents](#esp32_toc)

## Limitations of the RIOT port {#esp32_limitations}

The implementation of RIOT-OS for ESP32x SoCs has the following limitations
at the moment:

- Only **one core** (the PRO CPU) is used because RIOT does not support running
  multiple threads simultaneously.
- **Flash encryption** is not yet supported.

[Back to table of contents](#esp32_toc)

# Toolchain {#esp32_toolchain}

To build RIOT applications for ESP32x SoCs, the following components are required:

- ESP32 Toolchain including GCC, GDB and optionally OpenOCD and QEMU
- ESP32 SDK called ESP-IDF (Espressif IoT Development Framework)
- `esptool.py` for flashing

Principally, there are two ways to install and use the ESP32 toolchain, either

- using the **RIOT Docker build image**, see section
  [Using RIOT Docker Toolchain](#esp32_riot_docker_toolchain), or
- using a **local installation** of the ESP32 toolchain, see section
  [Using Local Toolchain Installation](#esp32_local_toolchain_installation).

[Back to table of contents](#esp32_toc)

## Using RIOT Docker Toolchain {#esp32_riot_docker_toolchain}

The easiest way to use the ESP32 toolchain is to use the RIOT Docker build
image. It is specially prepared for building RIOT applications for various
platforms and already has all the required tools and packages installed.
Details on how to setup Docker can be found in section
[Getting Started](https://doc.riot-os.org/getting-started.html#docker).

The building process using Docker comprises two steps:

1. Building the RIOT application **in Docker** using command:
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   $ sudo docker run --rm -i -t -u $UID -v $(pwd):/data/riotbuild riot/riotbuild
   riotbuild@container-id:~$ make BOARD= ...
   riotbuild@container_id:~$ exit
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2. Flashing the RIOT application **on the host system** using command
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   $ make flash-only BOARD=...
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Both steps can also be performed with a single command on the host system
by setting the `BUILD_IN_DOCKER` variable:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ BUILD_IN_DOCKER=1 make flash BOARD=...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp32_toc)

## Using Local Toolchain Installation {#esp32_local_toolchain_installation}

### Prerequisites

In addition to the common tools defined in section
[Getting Started - Common Tools](https://doc.riot-os.org/getting-started.html#compiling-riot),
the following tools or packages are required to install and use the ESP32
toolchain (Debian/Ubuntu package names):

- `curl`
- `python3`
- `python3-serial`
- `telnet`

### Script-based installation

The shell script `$RIOTBASE/dist/tools/esptools/install.sh` is used to
install Espressif's precompiled versions of the following tools:

- ESP32 vendor toolchain
- GDB for ESP32x SoCs based on Xtensa or RISC-V
- OpenOCD for ESP32 (for ESP32, ESP32-S2, ESP32-S3, ESP32-C3 and ESP32-H2)
- QEMU for ESP32x SoCs (for ESP32, ESP32-S3 and ESP32-C3)

`$RIOTBASE` defines the root directory of the RIOT repository. The shell
script takes an argument that specifies which tools to download and install:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ dist/tools/esptools/install.sh

Usage: install.sh <tool>
       install.sh gdb <platform>
       install.sh qemu <platform>
<tool> = all | gdb | openocd | qemu |
         esp8266 | esp32 | esp32c3 | esp32h2 | esp32s2 | esp32s3
<platform> = xtensa | riscv
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Thus, either all tools or only certain tools can be installed.

The ESP32x tools are installed within a subdirectory of the directory specified
by the environment variable `$IDF_TOOLS_PATH`. If the environment variable
`$IDF_TOOLS_PATH` is not defined, `$HOME/.espressif` is used as default.

Using the variable `IDF_TOOLS_PATH` and its default value `$HOME/.espressif` for
the toolchain installation in RIOT allows to reuse the tools that have already
been installed according to the section
["Get Started, Step 3. Set up the tools"](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html#get-started-set-up-tools).
if you have already used ESP-IDF directly.

### Using the toolchain

Once the ESP32x tools are installed in the directory specified by the
environment variable `$IDF_TOOLS_PATH`, the shell script
`$RIOTBASE/dist/tools/esptools/export.sh` can be sourced to export the
paths of the installed tools using again the environment variable
`$IDF_TOOLS_PATH`.
By writing `.` followed by a space in front of the script, the script
runs in the same environment as the current shell and sets the local
environment variables, which would otherwise be lost on termination
of the script.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ . dist/tools/esptools/export.sh

Usage: export.sh <tool>
       export.sh gdb <platform>
       export.sh qemu <platform>
<tool> = all | gdb | openocd | qemu |
         esp8266 | esp32 | esp32c3 | esp32h2 | esp32s2 | esp32s3
<platform> = xtensa | riscv
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All the tools required for building a RIOT application for ESP32x SoCs should then
be found in the path.

[Back to table of contents](#esp32_toc)

### Installation of the ESP32 SDK (ESP-IDF) {#esp32_installation_of_esp_idf}

RIOT-OS uses the ESP-IDF, the official SDK from Espressif, as part of the
build. It is downloaded as a package at build-time and there is no need to
install it separately.

### Installation of esptool.py (ESP flash programmer tool) {#esp32_installation_of_esptool}

The RIOT port uses the ESP flasher program `esptool.py` as a package directly
from [GitHub](https://github.com/espressif/esptool) within a virtual Python
environment.

For more information on `esptool.py`, please refer to the
[git repository](https://github.com/espressif/esptool).

[Back to table of contents](#esp32_toc)

# Flashing the Device {#esp32_flashing_the_device}

## Toolchain Usage {#esp32_toolchain_usage}

Once the toolchain is installed either as RIOT docker build image or as local
installation, a RIOT application can be compiled and flashed for an ESP32x
boards. For that purpose change to RIOT's root directory and execute the make
command, for example:

- RIOT Docker build image installation
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  $ BUILD_IN_DOCKER=1 DOCKER="sudo docker" \
    make flash BOARD=esp32-wroom-32 -C tests/sys/shell/ [Compile Options]
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- Local toolchain installation
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  $ make flash BOARD=esp32-wroom-32 -C tests/sys/shell/ [Compile Options]
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The `BOARD` variable in the example specifies the generic ESP32 board that uses
the ESP32-WROOM-32 module and option `-C` the directory of the application.

[Back to table of contents](#esp32_toc)

## Compile Options {#esp32_compile_options}

The compilation process can be controlled by a number of variables for the
make command:

<center>

Option      | Values                | Default       | Description
------------|-----------------------|---------------|------------
`CFLAGS`    | string                | empty         | Override default board and driver configurations, see section [Application-Specific Configurations](#esp32_application_specific_configurations).
`FLASH_MODE`| dout, dio, qout, qio  | dout          | Set the flash mode, see section [Flash Modes](#esp32_flash_modes)
`PORT`      | `/dev/tty*`           | `/dev/ttyUSB0`| Set the port for flashing the firmware.

</center><br>

Optional features of ESP32 can be enabled by `USEMODULE` definitions in the makefile of the
application. These are:

<center>

Module              | Description
--------------------|------------
esp_ble             | Enable the built-in Bluetooth LE module, see section [Bluetooth Interface](#esp32_esp_bluetooth_interface)
esp_eth             | Enable the Ethernet MAC (EMAC) interface as `netdev` network device, see section [Ethernet Network Interface](#esp32_ethernet_network_interface).
esp_gdb             | Enable the compilation with debug information for debugging with [QEMU and GDB](#esp32_qemu_mode_and_gdb) (`QEMU=1`) or via [JTAG interface with OpenOCD](#esp32_jtag_debugging).
esp_i2c_hw          | Use the hardware I2C implementation, see section [I2C Interfaces](#esp32_i2c_interfaces).
esp_ieee802154      | Enable the built-in IEEE 802.15.4 module, see section [IEEE 802.15.4 Interface](#esp32_esp_ieee802154_interface)
esp_idf_heap        | Use the ESP-IDF heap implementation, see section [ESP-IDF Heap Implementation](#esp32_esp_idf_heap_implementation).
esp_log_colored     | Enable colored log output, see section [Log output](#esp32_esp_log_module).
esp_log_startup     | Enable additional startup information, see section [Log output](#esp32_esp_log_module).
esp_log_tagged      | Add additional information to the log output, see section [Log output](#esp32_esp_log_module).
esp_now             | Enable the built-in WiFi module with the ESP-NOW protocol as `netdev` network device, see section [ESP-NOW Network Interface](#esp32_esp_now_network_interface).
esp_spi_oct         | Enable the optional SPI RAM in Octal SPI mode, see section [SPI RAM Modules](#esp32_spi_ram).
esp_qemu            | Generate an application image for QEMU, see section [QEMU Mode and GDB](#esp32_qemu_mode_and_gdb).
esp_rtc_timer_32k   | Enable RTC hardware timer with external 32.768 kHz crystal.
esp_spiffs          | Enable the optional SPIFFS drive in on-board flash memory, see section [SPIFFS Device](#esp32_spiffs_device).
esp_spi_ram         | Enable the optional SPI RAM, see section [SPI RAM Modules](#esp32_spi_ram).
esp_wifi            | Enable the built-in WiFi module as `netdev` network device in WPA2 personal mode, see section [WiFi Network Interface](#esp32_wifi_network_interface).
esp_wifi_ap         | Enable the built-in WiFi SoftAP module as `netdev` network device, see section [WiFi SoftAP Network Interface](#esp32_wifi_ap_network_interface).
esp_wifi_enterprise | Enable the built-in WiFi module as `netdev` network device in WPA2 enterprise mode, see section [WiFi Network Interface](#esp32_wifi_network_interface).

</center><br>

For example, to activate a SPIFFS drive in on-board flash memory, the makefile
of application has simply to add the `esp_spiffs` module to `USEMODULE` make
variable:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_spiffs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Modules can be also be activated temporarily at the command line when calling
the make command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE="esp_spiffs" make BOARD=...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp32_toc)

## Flash Modes {#esp32_flash_modes}

The `FLASH_MODE` make command variable determines the mode that is used for
flash access in normal operation.

The flash mode determines whether 2 data lines (`dio` and `dout`) or 4 data
lines (`qio` and `qout`) are used for flash addressing and data access. Since
one GPIO is required for each data line, `qio` or `qout` increases the
performance of SPI flash data transfers but uses two additional GPIOs.
That means these GPIOs are not available for other purposes in `qio` or `qout`
flash mode. If you can live with lower flash data transfer rates, you should
always use `dio` or `dout` to keep them free for other purposes:

- ESP32 GPIO9 and GPIO10
- ESP32-C3 GPIO12 and GPIO13
- ESP32-S2 GPIO27 and GPIO28
- ESP32-S3 GPIO27 and GPIO28

For more information about these flash modes, refer the documentation of
[esptool.py](https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/flash-modes.html).

@note On some modules and most boards, these additional GPIOs used in `qio`
or `qout` flash mode are not broken out and therefore not available.

[Back to table of contents](#esp32_toc)

## Log output {#esp32_esp_log_module}

The RIOT port for ESP32x SoCs implements a log module with a bunch of macros
to generate log output according to the interface as defined in
\ref core_util_log "system logging header". These macros support colored and
tagged log output.

The colored log output is enabled by module `esp_log_colored`. If colored log
output is enabled, log messages are displayed in color according to their type:
Error messages are displayed in red, warnings in yellow, information
messages in green and all other message types in standard color.

When the `esp_log_tagged` module is used, all log messages are tagged with
additional information: the type of message, the system time in ms, and the
module or function in which the log message is generated. For example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
I (663) [main_trampoline] main(): This is RIOT! (Version: 2019.10-devel-437-gf506a)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Either the `LOG_*` macros as defined in
\ref core_util_log "system logging header" or the tagged
version `LOG_TAG_*` of these macros can be used to produce tagged log output.
If the `LOG_*` macros are used, the function which generates the log message
is used in the tag while a `tag` parameter is used for the `LOG_TAG_*` macros.
For example,
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
LOG_ERROR("error message");
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
generates a log message in which the name of the calling function is used as
tag. With
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
LOG_TAG_ERROR("mod", "error message");
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
a log message with string `mod` in the tag is generated.

The `esp_log_startup` module can be used to enable additional information
about the boot process, the board configuration, the system configuration,
the CPU used by the system, and the available heap. These information may
help to detect problems during the startup. If the application does not
start as expected, this module should be used.

[Back to table of contents](#esp32_toc)

## ESP-IDF Heap Implementation {#esp32_esp_idf_heap_implementation}

ESP-IDF SDK provides a complex heap implementation that supports multiple heap
segments in different memory areas such as DRAM, IRAM, and PSRAM. Whenever you
want to use these memory areas as heap, you have to use the heap implementation
from the ESP-IDF SDK. ESP-IDF heap is not used by default. To use it, it has to
be enabled by the makefile of the application:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_heap
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note
ESP-IDF heap implementation is used by default, when the following modules
are used: `esp_spi_ram`, `esp_wifi_any`

[Back to table of contents](#esp32_toc)

# Common Peripherals {#esp32_peripherals}

ESP32x SoCs have a lot of peripherals that are not all supported by the
RIOT port. This section describes the supported peripherals and how they have
to be configured.

[Back to table of contents](#esp32_toc)

## GPIO pins {#esp32_gpio_pins}

The number of GPIOs and their usage depends on the respective ESP32x SoC family.
For details, see:

- \ref esp32_gpio_pins_esp32 "ESP32"
- \ref esp32_gpio_pins_esp32c3 "ESP32-C3"
- \ref esp32_gpio_pins_esp32h2 "ESP32-H2"
- \ref esp32_gpio_pins_esp32s2 "ESP32-S2"
- \ref esp32_gpio_pins_esp32s3 "ESP32-S3"

[Back to table of contents](#esp32_toc)

## ADC Channels {#esp32_adc_channels}

ESP32x SoCs integrate up to two SAR ADCs (ADC1 and ADC2). The bit width of the
ADC devices, the number of channels per device and the GPIOs that can be
used as ADC channels depend on the respective ESP32x SoC family. For
details, see:

- \ref esp32_adc_channels_esp32 "ESP32"
- \ref esp32_adc_channels_esp32c3 "ESP32-C3"
- \ref esp32_adc_channels_esp32h2 "ESP32-H2"
- \ref esp32_adc_channels_esp32s2 "ESP32-S2"
- \ref esp32_adc_channels_esp32s3 "ESP32-S3"

#ADC_GPIOS in the board-specific peripheral configuration defines the
list of GPIOs that can be used as ADC channels on the board, for example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define ADC_GPIOS   { GPIO0, GPIO2, GPIO4 }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Thereby the order of the listed GPIOs determines the mapping between the
ADC lines of the RIOT and the GPIOs. The maximum number of GPIOs in the
list is #ADC_NUMOF_MAX. The board specific configuration of #ADC_GPIOS
can be overridden by
[Application specific configurations](#esp32_application_specific_configurations).

The number of defined ADC channels #ADC_NUMOF is determined automatically
from the #ADC_GPIOS definition.

@note As long as the GPIOs listed in #ADC_GPIOS are not initialized as ADC
      channels with the #adc_init function, they can be used for other
      purposes.

With the function #adc_set_attenuation an attenuation of the input signal
can be defined separately for each ADC channel.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
extern int adc_set_attenuation(adc_t line, adc_attenuation_t atten);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This leads to different measurable maximum values for the voltage at the input.
The higher the attenuation is, the higher the voltage measured at the input
can be.

The attenuation can be set to 4 fixed values 0 dB, 2.5/3 dB, 6 dB and
11/12 dB, where 11 dB respectively 12 dB is the default attenuation.

<center>

Attenuation | Voltage Range     | Symbol
-----------:|-------------------|---------------------------
      0 dB  | 0 ... 1.1V (Vref) | `ADC_ATTEN_DB_0`
2.5 / 3 dB  | 0 ... 1.5V        | `ADC_ATTEN_DB_2_5`
      6 dB  | 0 ... 2.2V        | `ADC_ATTEN_DB_6`
11 / 12 dB  | 0 ... 3.3V        | `ADC_ATTEN_DB_12` (default)

</center><br>

@note The reference voltage Vref can vary from device to device in the range
      of 1.0V and 1.2V.

@note ADC2 is also used by the WiFi module. The GPIOs connected to ADC2 are
      therefore not available as ADC channels if the modules `esp_wifi` or
      `esp_now` are used.

[Back to table of contents](#esp32_toc)

## DAC Channels {#esp32_dac_channels}

Some ESP32x SoCs support 2 DAC lines at predefined GPIOs, depending on the
respective ESP32x SoC family. These DACs have a width of 8 bits and produce
voltages in the range from 0 V to 3.3 V (VDD_A). The 16 bit DAC values
given as parameter of function #dac_set are down-scaled to 8 bit.

The GPIOs that can be used as DAC channels for a given board are defined by
the `#DAC_GPIOS` macro in the board-specific peripheral configuration.
The specified GPIOs in the list must match the predefined GPIOs that can be
used as DAC channels on the respective ESP32x SoC.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define DAC_GPIOS   { GPIO25, GPIO26 }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This configuration can be changed by
[application-specific configurations](#esp32_application_specific_configurations).

The order of the listed GPIOs determines the mapping between the RIOT's
DAC lines and the GPIOs. The maximum number of GPIOs in the list is
#DAC_NUMOF_MAX.

#DAC_NUMOF is determined automatically from the #DAC_GPIOS definition.

@note As long as the GPIOs listed in #DAC_GPIOS are not initialized
as DAC channels with the #dac_init function, they can be used for other
purposes.

DACs are currently only supported for the \ref esp32_dac_channels_esp32
"ESP32 SoC" and the \ref esp32_dac_channels_esp32s2 "ESP32-S2 SoC" variant.

[Back to table of contents](#esp32_toc)

## I2C Interfaces {#esp32_i2c_interfaces}

ESP32x SoCs integrate up to two I2C hardware interfaces.

The board-specific configuration of the I2C interface I2C_DEV(n) requires
the definition of

`I2Cn_SPEED`, the bus speed for I2C_DEV(n),
`I2Cn_SCL`, the GPIO used as SCL signal for I2C_DEV(n), and
`I2Cn_SDA`, the GPIO used as SDA signal for I2C_DEV(n),

where `n` can be 0 or 1. If they are not defined, the I2C interface
I2C_DEV(n) is not used, for example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define I2C0_SPEED  I2C_SPEED_FAST
#define I2C0_SCL    GPIO22
#define I2C0_SDA    GPIO21

#define I2C1_SPEED  I2C_SPEED_NORMAL
#define I2C1_SCL    GPIO13
#define I2C1_SDA    GPIO16
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The board-specific pin configuration of I2C interfaces can be changed by
[application specific configurations](#esp32_application_specific_configurations)
by overriding the according `I2Cn_*` symbols.

The default configuration of I2C interfaces for ESP32x SoC boards depend
on used ESP32x SoC family, for details see:

- \ref esp32_i2c_interfaces_esp32 "ESP32"
- \ref esp32_i2c_interfaces_esp32c3 "ESP32-C3"
- \ref esp32_i2c_interfaces_esp32h2 "ESP32-H2"
- \ref esp32_i2c_interfaces_esp32s2 "ESP32-S2"
- \ref esp32_i2c_interfaces_esp32s3 "ESP32-S3"

@note
- To ensure that the `I2Cn_*` symbols define the configuration for
  I2C_DEV(n), the definition of the configuration of I2C interfaces
  I2C_DEV(n) must be in continuous ascending order of `n`.
  That is, if I2C_DEV(1) is used by defining the `I2C1_*` symbols,
  I2C_DEV(0) must also be used by defining the `I2C0_*` symbols.
- The GPIOs listed in the configuration are only initialized as I2C
  signals when the `periph_i2c` module is used. Otherwise they are not
  allocated and can be used for other purposes.
- The same configuration is used when the I2C bit-banging software
  implementation is used by enabling module `esp_i2c_sw` (default).

The number of used I2C interfaces #I2C_NUMOF is determined automatically
from board-specific peripheral definitions of I2C_DEV(n).

Beside the I2C hardware implementation, a I2C bit-banging protocol software
implementation can be used. This implementation allows bus speeds up to
1 Mbps (`#I2C_SPEED_FAST_PLUS`). It can be activated by adding
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_i2c_sw
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
to application's makefile. The Disadvantage of the software implementation
is that it uses busy waiting.

@note The hardware implementation seems to be very poor and faulty. I2C
      commands in the I2C command pipeline are sporadically not executed.
      A number of ACK errors and timeouts caused by protocol errors are
      the result. The hardware implementation is recommended only if
      they can be tolerated. Therefore, the software implementation is
      used by default.

[Back to table of contents](#esp32_toc)

## PWM Channels {#esp32_pwm_channels}

The PWM peripheral driver for ESP32x SoCs uses the LED PWM Controller (LEDC)
module for implementation. The LEDC module has either 1 or 2 channel groups
with 6 or 8 channels each, where the first channel group comprises the
low-speed channels and the second channel group comprises the high-speed
channels. The difference is that changes in the configuration of the
high-speed channels take effect with the next PWM cycle, while the changes
in the configuration of the low-speed channels must be explicitly updated
by a trigger.

The low-speed channel group always exists while the existence high-speed
channel depends on respective ESP32x SoC family. For details, see:

- \ref esp32_pwm_channels_esp32 "ESP32"
- \ref esp32_pwm_channels_esp32c3 "ESP32-C3"
- \ref esp32_pwm_channels_esp32h2 "ESP32-H2"
- \ref esp32_pwm_channels_esp32s2 "ESP32-S2"
- \ref esp32_pwm_channels_esp32s3 "ESP32-S3"

Each channel group has 4 timers which can be used as clock source by the
channels of the respective channel group. Thus it would be possible to
define a maximum of 4 virtual PWM devices in RIOT per channel group with
different frequencies and resolutions. However, regardless of whether the
LEDC module of the ESP32x SoC has one or two channel groups, the PWM driver
implementation only allows the available channels to be organized into
up to 4 virtual PWM devices.

The assignment of the available channels to the virtual PWM devices is
done in the board-specific peripheral configuration by defining the
macros `PWM0_GPIOS`, `PWM1_GPIOS`, `PWM2_GPIOS` and `PWM3_GPIOS` These
macros specify the GPIOs that are used as channels for the 4 possible
virtual PWM devices PWM_DEV(0) ... PWM_DEV(3) in RIOT, for example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define PWM0_GPIOS  { GPIO0, GPIO2, GPIO4, GPIO16, GPIO17 }
#define PWM1_GPIOS  { GPIO27, GPIO32, GPIO33 }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This configuration can be changed by
[application-specific configurations](#esp32_application_specific_configurations).

The mapping of the GPIOs as channels of the available channel groups and
channel group timers is organized by the driver automatically as follows:

<center>

Macro        | 1 Channel Group       | 2 Channel Groups       | Timer
-------------|-----------------------|------------------------|---------------
`PWM0_GPIOS` | `LEDC_LOW_SPEED_MODE` | `LEDC_LOW_SPEED_MODE`  | `LEDC_TIMER_0`
`PWM1_GPIOS` | `LEDC_LOW_SPEED_MODE` | `LEDC_HIGH_SPEED_MODE` | `LEDC_TIMER_1`
`PWM2_GPIOS` | `LEDC_LOW_SPEED_MODE` | `LEDC_LOW_SPEED_MODE`  | `LEDC_TIMER_2`
`PWM3_GPIOS` | `LEDC_LOW_SPEED_MODE` | `LEDC_HIGH_SPEED_MODE` | `LEDC_TIMER_3`

</center>

For example, if the LEDC module of the ESP32x SoC has two channel groups,
two virtual PWM devices with 2 x 6 (or 8) channels could be used by defining
'PWM0_GPIOS' and 'PWM1_GPIOS' with 6 (or 8) GPIOs each.

The number of used PWM devices #PWM_NUMOF is determined automatically from the
definition of `PWM0_GPIOS`, `PWM1_GPIOS`, `PWM2_GPIOS` and `PWM3_GPIOS`.

@note
- The total number of channels defined for a channel group must not exceed
  #PWM_CH_NUMOF_MAX
- The definition of `PWM0_GPIOS`, `PWM1_GPIOS`, `PWM2_GPIOS` and
  `PWM3_GPIOS` can be omitted. However, to ensure that `PWMn_GPIOS` defines
  the configuration for PWM_DEV(n), the PWM channels must be defined in
  continuous ascending order from `n`. That means, if `PWM1_GPIOS` is
  defined, `PWM0_GPIOS` must be defined before, and so on. So a minimal
  configuration would define all channels by `PWM0_GPIOS` as PWM_DEV(0).
- The order of the GPIOs in these macros determines the mapping between
  RIOT's PWM channels and the GPIOs.
- As long as the GPIOs listed in `PWM0_GPIOS`, `PWM1_GPIOS`,
  `PWM2_GPIOS` and `PWM3_GPIOS` are not initialized as PWM channels with
  the #pwm_init function, they can be used other purposes.

[Back to table of contents](#esp32_toc)

## SDMMC Interfaces {#esp32_sdmmc_interfaces}

ESP32 and ESP32-S3 variants integrate a SD/MMC host controller which supports
two slots for

- SD Memory Cards,
- SDIO Cards and
- MMC/eMMCs.

The SD/MMC host controller on the ESP32 variant

- supports 1-bit, 4-bit and 8-bit data bus width for slot 0 (@ref SDMMC_SLOT_0),
- supports 1-bit and 4-bit data bus width for slot 1 (@ref SDMMC_SLOT_1),
- uses direct I/O for SD/MMC signals so that the GPIOs used for the slots are
  therefore fixed.

The SD/MMC host controller on the ESP32-S3 variant
- supports 1-, 4- and 8-bit data bus width for both slots
- uses the GPIO matrix to connect the SD/MMC signals to the GPIOs so that
  all pins are configurable.

@note Since the GPIOs are fixed on the ESP32 variant and the same GPIOs are
used for slot 0 and the flash, slot 0 cannot be used on ESP32 variant.

The board-specific configuration is realized by defining the array
@ref sdmmc_config
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
static const sdmmc_conf_t sdmmc_config[] = {
    {
        .slot = SDMMC_SLOT_1,
        .cd = GPIO_UNDEF,
        .wp = GPIO_UNDEF,
        ...
    },
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
and the macro `SDMMC_NUMOF`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define SDMMC_NUMOF 1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

where the value of @ref SDMMC_NUMOF must correspond to the number of elements
in @ref sdmmc_config.

While for the ESP32 variant it is sufficient to define the data bus width, used
GPIOs have to be defined in the configuration for the ESP32-S3 variant instead.
For details of ESP32x variant specific configuration, see:

- \ref esp32_sdmmc_interfaces_esp32 "ESP32"
- \ref esp32_sdmmc_interfaces_esp32s3 "ESP32-S3"

If the board supports a Card Detect pin or a Write Protect pin, the
corresponding GPIOs have to be defined in @ref sdmmc_conf_t::cd and
@ref sdmmc_conf_t::wp. Otherwise they have to be set to undefined
(@ref GPIO_UNDEF).

## SPI Interfaces {#esp32_spi_interfaces}

ESP32x SoCs have up to four SPI controllers dependent on the specific ESP32x
SoC variant (family):

- Controller SPI0 is reserved for caching external memories like Flash
- Controller SPI1 is reserved for external memories like PSRAM
- Controller SPI2 can be used as general purpose SPI (GPSPI)
- Controller SPI3 can be used as general purpose SPI (GPSPI)

The controllers SPI0 and SPI1 share the same bus signals and can only
operate in memory mode on most ESP32x SoC variants. Therefore, depending on
the specific ESP32x SoC family, a maximum of two SPI controllers can be used
as peripheral interfaces:

- Controller SPI2 is identified by `SPI2_HOST` (also called FSPI or HSPI)
- Controller SPI3 is identified by `SPI3_HOST` (also called VSPI)

In former ESP-IDF versions, SPI interfaces were identified by the alias
names `FSPI`, `HSPI` and `VSPI`, which are sometimes also used in data
sheets. These alias names have been declared obsolete in ESP-IDF.

SPI interfaces could be used in quad SPI mode, but RIOT's low level
device driver doesn't support it.

The board-specific configuration of the SPI interface SPI_DEV(n) requires
the definition of

- `SPIn_CTRL`, the SPI controller (`SPI_HOST2`/`SPI_HOST3`) used for SPI_DEV(n),
- `SPIn_SCK`, the GPIO used as clock signal used for SPI_DEV(n)
- `SPIn_MISO`, the GPIO used as MISO signal used for SPI_DEV(n)
- `SPIn_MOSI`, the GPIO used as MOSI signal used for SPI_DEV(n), and
- `SPIn_CS0`, the GPIO used as CS signal for SPI_DEV(n) when the `cs`
  parameter in #spi_acquire is #GPIO_UNDEF,

where `n` can be 0 and 1. If they are not defined, the according SPI interface
SPI_DEV(n) is not used, for example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define SPI0_CTRL   SPI3_HOST   // VSPI could also be used on ESP32 variant
#define SPI0_SCK    GPIO18      // SCK  signal
#define SPI0_MISO   GPIO19      // MISO signal
#define SPI0_MOSI   GPIO23      // MOSI signal
#define SPI0_CS0    GPIO5       // CS0  signal

#define SPI1_CTRL   SPI2_HOST   // HSPI could also be used here on ESP32 variant
#define SPI1_SCK    GPIO14      // SCK  Camera
#define SPI1_MISO   GPIO12      // MISO Camera
#define SPI1_MOSI   GPIO13      // MOSI Camera
#define SPI1_CS0    GPIO15      // CS0  Camera
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The pin configuration of SPI interfaces can be changed by
[application specific configurations](#esp32_application_specific_configurations)
by overriding the according `SPIn_*` symbols.

The default configuration of SPI interface for ESP32x SoC boards depend
on used ESP32x SoC family, for details see:

- \ref esp32_spi_interfaces_esp32 "ESP32"
- \ref esp32_spi_interfaces_esp32c3 "ESP32-C3"
- \ref esp32_spi_interfaces_esp32h2 "ESP32-H2"
- \ref esp32_spi_interfaces_esp32s2 "ESP32-S2"
- \ref esp32_spi_interfaces_esp32s3 "ESP32-S3"

@note
- To ensure that the `SPIn_*` symbols define the configuration for
  SPI_DEV(n), the definition of the configuration of SPI interfaces
  SPI_DEV(n) must be in continuous ascending order of `n`.
  That is, if SPI_DEV(1) is used by defining the `SPI1_*` symbols,
  SPI_DEV(0) must also be used by defining the `SPI0_*` symbols.
- The order in which the available interfaces `SPI2_HOST` (alias `HSPI` or
  `FSP`) and `SPI3_HOST` (alias `HSPI`) are assigned doesn't matter.
- The GPIOs listed in the configuration are only initialized as SPI
  signals when the `periph_spi` module is used. Otherwise they are not
  allocated and can be used for other purposes.

`#SPI_NUMOF` is determined automatically from the board-specific
peripheral definitions of `SPI_DEV(n)`.

[Back to table of contents](#esp32_toc)

## Timers {#esp32_timers}

There are two different implementations for hardware timers.

- **Timer Module implementation** Depending on the ESP32x SoC variant
  (family) it provides up to 4 high speed timers, where 1 timer is used for
  system time. The remaining timer devices with **1 channel** each can be
  used as RIOT timer devices with a clock rate of 1 MHz.
- **Counter implementation** Dependent on the ESP32x SoC variant (family),
  the MCU has up to 3 CCOMPARE (cycle compare) registers. Two of them can be
  used to implement up to **2 timer devices** with **1 channel** each and a
  clock rate of 1 MHz. This is a feature of Xtensa-based ESP32x SoC variants.

By default, the timer module is used. To use the counter implementation, add
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_hw_counter
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
to application's makefile.

The number of timers and available timer implementations depend on used
ESP32x SoC family, for details see:

- \ref esp32_timers_esp32 "ESP32"
- \ref esp32_timers_esp32c3 "ESP32-C3"
- \ref esp32_timers_esp32h2 "ESP32-H2"
- \ref esp32_timers_esp32s2 "ESP32-S2"
- \ref esp32_timers_esp32s3 "ESP32-S3"

Timers are MCU built-in features and not board-specific. There is nothing to
be configured.

[Back to table of contents](#esp32_toc)

## RTT implementation {#esp32_rtt_counter}

The RTT peripheral low-level driver provides a RTT (Real Time Timer) with
a frequency of 32.768 kHz. It either uses the RTC hardware timer if an
external 32.768 kHz crystal is connected to the ESP32x SoC or the
PLL-controlled 64-bit microsecond system timer to emulate the RTC timer.

Whether an external 32.768 kHz crystal is connected to the ESP32x SoC is
specified as a feature by the board definition using the pseudomodule
`esp_rtc_timer_32k`. If the feature `esp_rtc_timer_32k` is defined but the
external 32.768 kHz crystal is not recognized during startup, the
PLL controlled 64 bit microsecond system timer is used to emulate the
RTC timer.

The RTT is retained during light and deep sleep as well as during a restart.
The RTC hardware timer is used for this purpose, regardless of whether an
external 32.768 kHz crystal is connected to the ESP32x SoC or the internal
150 kHz RC oscillator is used. All current timer values are saved in the
RTC memory before entering a sleep mode or restart and are restored after
when waking up or restarting.

@note The RTT implementation is also used to implement a RTC (Real Time Clock)
peripheral. For this purpose the module `rt_rtc` is automatically enabled
when the feature `periph_rtc` is used.

[Back to table of contents](#esp32_toc)

## UART Interfaces {#esp32_uart_interfaces}

ESP32x SoCs integrate up to three UART devices, depending on the specific
ESP32x SoC variant (family).

The pin configuration of the UART device UART_DEV(n) is defined in the
board-specific peripheral configuration by

- `UARTn_TXD`, the GPIO used as TxD signal for UART_DEV(n), and
- `UARTn_RXD`, the GPIO used as RxD signal for UART_DEV(n),

where `n` can be in range of 0 and UART_NUMOF_MAX-1. If they are not defined,
the according UART interface UART_DEV(n) is not used, for example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define UART1_TX    GPIO10  // TxD signal of UART_DEV(1)
#define UART1_RX    GPIO9   // RxD signal of UART_DEV(1)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The pin configuration of UART interfaces can be changed by
[application specific configurations](#esp32_application_specific_configurations)
by overriding the according `UARTn_*` symbols.

The default configuration of the UART interfaces for ESP32x SoC boards depend
on used ESP32x SoC family, for details see:

- \ref esp32_uart_interfaces_esp32 "ESP32"
- \ref esp32_uart_interfaces_esp32c3 "ESP32-C3"
- \ref esp32_uart_interfaces_esp32h2 "ESP32-H2"
- \ref esp32_uart_interfaces_esp32s2 "ESP32-S2"
- \ref esp32_uart_interfaces_esp32s3 "ESP32-S3"

@note To ensure that the `UARTn_*` symbols define the configuration for
      UART_DEV(n), the configuration of the UART interfaces UART_DEV(n)
      must be in continuous ascending order of `n`. That is, if UART_DEV(1)
      is to be used by defining the `UART1_*` symbols, UART_DEV(0) must also
      be used by defining the `UART0_*` symbols, and if UART_DEV(2)
      is to be used by defining the `UART2_*` symbols, UART_DEV(0) and
      UART_DEV(1) must also be used by defining the `UART0_*` and
     `UART1_*` symbols

#UART_NUMOF is determined automatically from the board-specific peripheral
configuration for UART_DEV(n).

UART_DEV(0) has usually a fixed pin configuration that is used by all
ESP32x boards as standard configuration for the console. The GPIOs
used for UART_DEV(0) depend on the ESP32x SoC family.

[Back to table of contents](#esp32_toc)

## CAN Interfaces {#esp32_can_interfaces}

ESP32x SoCs intregate a Two-Wire Automotive Interface (TWAI) controller
which is compatible with the NXP SJA1000 CAN controller. Thus, it is
CAN 2.0B specification compliant and supports two message formats:

- Base Frame Format (11-bit ID)
- Extended Frame Format (29-bit ID)

@note
- The TWAI controller does not support CAN-FD and is not CAN-FD tolerant.
- The TWAI controller does not support SJA1000's sleep mode and wake-up
  functionality.

As with the SJA1000, the TWAI controller provides only the data link layer
and the physical layer signaling sublayer. Therefore, depending on physical
layer requirements, an external CAN transceiver is required which converts
the `CAN-RX` and `CAN-TX` signals of the TWAI controller into `CAN_H` and
`CAN_L` bus signals, e.g., the MCP2551 or SN65HVD23X transceiver for
compatibility with ISO 11898-2.

If module `periph_can` is used, the low-level CAN driver for the
TWAI controller is enabled. It provides a CAN DLL device that can be
used with RIOT's CAN protocol stack. It uses the TWAI CANcontroller
in SJA1000 PeliCAN mode. Please refer the
[SJA1000 Datasheet](https://www.nxp.com/documents/data_sheet/SJA1000.pdf)
for detailed information about the TWAI controller and its programming.

The pin configuration of the CAN transceiver interface is usually defined
in board specific peripheral configuration by

- `#CAN_TX`, the GPIO used as TX transceiver signal, and
- `#CAN_RX`, the GPIO used as RX transceiver signal.

Example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define CAN_TX      GPIO10      // CAN TX transceiver signal
#define CAN_RX      GPIO9       // CAN RX transceiver signal
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the pin configuration is not defined, the following default configuration
is used which can be overridden by the application, see section
[Application-Specific Configurations](#esp32_application_specific_configurations).

Device  |Signal|Pin     |Symbol         |Remarks
:-------|:-----|:-------|:--------------|:----------------
CAN     | TX   | GPIO5  | `CAN_TX`      | optional, can be overridden
CAN     | RX   | GPIO35 | `CAN_RX`      | optional, can be overridden

If the board has an external transceiver module connected to the
ESP32x SoC on-board, module `periph_can` should be provided as feature
in board's `Makefile.features`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FEATURES_PROVIDED += periph_can     # CAN peripheral interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Otherwise, the application has to add the `periph_can` module in its makefile
when needed.

[Back to table of contents](#esp32_toc)

## Power Management {#esp32_power_management}

### Power Modes

The RIOT port for ESP32x SoCs implements RIOT's layered power management. It
supports the following operating modes:

- The **Modem-sleep** mode is the default operating mode when the WiFi
  interface is disabled.
- The **Active** mode is the default operating mode when the WiFi interface
  is used by either the `esp-wifi` or `esp-now` module.
- In **Light-sleep** mode, the CPUs including peripherals are stalled, but
  the SRAM is retained. The system can continue when it returns from this mode.
- In **Deep-sleep** the CPU and the SRAM are powered down. The RTC memory can
  be retained. The system must be restarted when it returns from this mode.

Since the peripherals are not working during _Light-sleep_ and _Deep-sleep_, the
CPU cannot be woken up by internal interrupt sources such as timers. Therefore,
RIOT's layered power management can't select them as idle power mode. They are
therefore blocked for normal operation. The application has to select them
explicitly using the `#pm_set` function. RIOT's layered power management
can only select either _Modem-sleep_ or _Active_ as the lowest unblocked mode.

But also in _Modem-sleep_ or _Active_ mode, the lowest possible power level
is used. For this purpose, the Xtensa ISA instruction `waiti` is used,
which saves power by setting the current interrupt level, turning off the
processor logic and waiting for an interrupt.

[Back to table of contents](#esp32_toc)

### Using Power Modes

_Modem-sleep_ mode and _Active_ mode are the default operating modes
dependent on whether the WiFi interface is used. They are selected
automatically by the system.

To enter the _Light-sleep_ or the _Deep-sleep_ mode, function `#pm_set` has
to be used with the according mode `ESP_PM_LIGHT_SLEEP` or `ESP_PM_DEEP_SLEEP`
as parameter. To exit from these modes, several wake-up sources can be used.

[Back to table of contents](#esp32_toc)

#### Wake-up Sources in _Light-sleep_

Possible wake-up sources for the _Light-sleep_ mode are:

- RTC timer (set the RTC timer alarm using `#rtc_set_alarm` before calling `#pm_set`)
- GPIOs that are configured as input with enabled interrupt
- RxD signal of UART0 and/or UART1 (configured with `ESP_PM_WUP_UART0` and
  `ESP_PM_WUP_UART1`)

@note Since the digital core (MCU) is stalled during _Light-sleep_, it
is not possible to use timers like `periph_timer` or `ztimer` as wake-up source.

@warning
Since only level interrupts are supported in _Light-sleep_ mode,
defined edge interrupts of type `#GPIO_RISING` and `#GPIO_FALLING` are
implicitly mapped to `GPIO_HIGH`  and `GPIO_LOW`, respectively, when
entering _Light-sleep_ mode.

[Back to table of contents](#esp32_toc)

#### Wake-up Sources in _Deep-sleep_ Mode

Possible Wake-up sources for the _Deep-sleep_ mode are:

- RTC timer (set the RTC timer alarm using `#rtc_set_alarm` before calling `#pm_set`)
- RTC GPIOs (configured by `ESP_PM_WUP_PINS` and `ESP_PM_WUP_LEVEL`)

@note RTC GPIOs are the GPIOs that are realized by the RTC unit and can also
be used as ADC channels. See section [GPIO pins](#esp32_gpio_pins) and
[ADC Channels](#esp32_adc_channels) for more information.

[Back to table of contents](#esp32_toc)

### Configuration

Several definitions can be used during compile time to configure the
_Light-sleep_ and the _Deep-sleep_ mode:

<center>

Parameter        | Default                  | Mode  | Description
:----------------|:-------------------------|:------|:------------
ESP_PM_GPIO_HOLD | not defined              | Deep  | Hold GPIO output level in _Deep-sleep_ mode if defined
ESP_PM_WUP_PINS  | none                     | Deep  | GPIOs used as wake-up source from _Deep-sleep_ mode
ESP_PM_WUP_LEVEL | ESP_PM_WUP_PINS_ANY_HIGH | Deep  | Level for wake-up pins to wake-up from _Deep-sleep_ mode
ESP_PM_WUP_UART0 | disabled                 | Light | Positive UART0 RxD signal edges to wake-up
ESP_PM_WUP_UART1 | disabled                 | Light | Positive UART1 RxD signal edges to wake-up

</center><br>

- If `ESP_PM_GPIO_HOLD` is defined, GPIOs hold their last output level
  when entering _Deep-sleep_ mode. Please note that only RTC GPIOs
  can hold their output value in _Deep-sleep_ mode.
  When restarting after a deep sleep, the GPIOs are reset to their default
  configuration, which is usually the input-only mode. This means that the
  output level of the GPIOs in output mode may change for up to 150 ms until
  the GPIOs are reconfigured by the application in output mode. If a continuous
  output level for such GPIOs is important, external pullups/pulldowns should
  be used for these GPIOs to pull them to a specific level during deep sleep
  and restart instead of defining `ESP_PM_GPIO_HOLD`.
- `ESP_PM_WUP_PINS` specifies either a single RTC GPIO or a comma separated
  list of RTC GPIOs that are used as wake-up source in _Deep-sleep_ mode.
- `ESP_PM_WUP_LEVEL` specifies the level for the wake-up pins in _Deep-sleep_
  mode:
  - `ESP_PM_WUP_PINS_ANY_HIGH` (default) - The system is woken up when any of
     the GPIOs specified in `ESP_PM_WUP_PINS` becomes HIGH.
  - `ESP_PM_WUP_PINS_ANY_LOW` - The system is woken up when any of
     the GPIOs specified in `ESP_PM_WUP_PINS` becomes LOW.
  - `ESP_PM_WUP_PINS_ALL_LOW` - The system is woken up when all GPIOs specified
    in `ESP_PM_WUP_PINS` become LOW.
- `ESP_PM_WUP_UART0` and `ESP_PM_WUP_UART1` define the number of positive
  edges of the RxD signal of the respective UART that are necessary to wake
  up the system in the _Light-sleep_ mode. The value must be greater than 2,
  otherwise UART is not activated as wake-up source. The specified value is
  reduced by 2 so that `ESP_PM_WUP_UART0` or `ESP_PM_WUP_UART1` plus 2 is
  the number of positive edges required to wake up.

@note
- Wake-up from _Light-sleep_ mode works for any of the GPIOs defined as input
  with enabled interrupt.
- `ESP_PM_GPIO_HOLD` is not available on ESP32-C3 variant.
- `ESP_PM_GPIO_HOLD` works only for RTC GPIOs.
- `ESP_PM_WUP_PINS_ANY_LOW` is not available on ESP32 variant.
- `ESP_PM_WUP_PINS_ALL_LOW` is only available on ESP32 variant, all other
  ESP32x variants support only `ESP_PM_WUP_PINS_ANY_LOW` instead.

In the following example the system shall be woken up from _Deep-sleep_ if
the pulled-up pin `GPIO25` (`ESP_PM_WUP_PINS=GPIO25`) goes LOW
(`ESP_PM_WUP_LEVEL=ESP_PM_WUP_PINS_ALL_LOW`). The last GPIO output values
are held (`ESP_PM_GPIO_HOLD`) in _Deep-sleep_ mode. From _Light-sleep_ the
system can be woken up by any of the GPIOs defined as input with enabled
interrupt or if the RxD signal of UART0 goes HIGH at least 4 times
(`ESP_PM_WUP_UART0=6`).
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS='-DESP_PM_WUP_PINS=GPIO25 -DESP_PM_WUP_LEVEL=ESP_PM_WUP_PINS_ALL_LOW \
        -DESP_PM_WUP_UART0=6 -DESP_PM_GPIO_HOLD' \
make BOARD=esp32-wroom-32 -C tests/periph/pm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp32_toc)

### Saving Data in _Deep-sleep_ Mode

In _Deep-sleep_ mode the SRAM is powered down. However, the slow RTC memory
can be retained. Therefore, data that must be retained during _Deep-sleep_ and
the subsequent system restart, must be stored in the slow RTC memory. For
that purpose, use
- `__attribute__((section(".rtc.bss")))` to place uninitialized
   data in section `.rtc.bss`, and
- `__attribute__((section(".rtc.data")))` to place initialized
  data in section `.rtc.data`.

For example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
static int _i_value __attribute__((section(".rtc.bss")));      // set to 0 at power on
static int _u_value __attribute__((section(".rtc.data"))) = 1; // initialized
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp32_toc)

## Other Peripherals {#esp32_other_peripherals}

The RIOT port for ESP32x SoCs also supports:

- hardware number generator with 32 bit
- CPU-ID function
- Vref measurement function

[Back to table of contents](#esp32_toc)

# Special On-board Peripherals {#esp32_special_on_board_peripherals}

## SPI RAM Modules {#esp32_spi_ram}

Dependent on the ESP32x SoC variant (family), external SPI RAM can be
connected to the SPI interface that is driven by the SPI1 controller
(`SPI1_HOST`). For example, all boards that use the
[ESP32-WROVER modules](https://www.espressif.com/sites/default/files/documentation/esp32-wrover-e_esp32-wrover-ie_datasheet_en.pdf)
have already integrated such SPI RAM. The connected SPI RAM is treated
as PSRAM (pseudo-static RAM) and is integrated into the heap.

However, the external SPI RAM requires 4 data lines and thus can only be
used in `qout` (quad output) or `qio` (quad input/output) flash mode, which
makes GPIO9 and GPIO10 unavailable for other purposes. Therefore, if needed,
the SPI RAM must be explicitly enabled in the makefile of the application.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_spi_ram
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note
- When the SPI RAM is enabled using the `esp_spi_ram`, the ESP32x SoC uses
  four data lines to access the external SPI RAM in `qout` (quad output)
  flash mode. Therefore, GPIO9 and GPIO10 are used as SPI data lines and
  are not available for other purposes.
- Enabling SPI RAM for modules that don't have SPI RAM may lead to boot
  problems for some modules. For others it simply throws an error message.

Newer ESP32x SoC variants (families) like the ESP32-S3 support the Octal
SPI mode for Flash and SPI RAMs. Depending on the chip or module used,
it must be specified in the board definition whether the optional SPI RAM
is used in Octal SPI mode (feature `esp_spi_oct`). In this case additional
GPIOs are needed as data lines and are not available for other purposes.
If the feature `esp_spi_oct` is defined for a board, the pseudomodule
`esp_spi_oct` is automatically enabled when the SPI RAM is used.

[Back to table of contents](#esp32_toc)

## SPIFFS Device {#esp32_spiffs_device}

The RIOT port for ESP32x SoCs implements a MTD system drive `mtd0` using the
on-board SPI flash memory. This MTD system drive can be used together with
SPIFFS and VFS to realize a persistent file system.

To use the MTD system drive with SPIFFS, the `esp_spiffs` module has to be
enabled in the makefile of the application:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_spiffs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When SPIFFS is enabled, the MTD system drive is formatted with SPIFFS the
first time the system is started. The start address of the MTD system drive
in the SPI flash memory is defined by the board configuration:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#define SPI_FLASH_DRIVE_START  0x200000
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
If this start address is set to 0, as in the default board configuration,
the first possible multiple of 0x100000 (1 MiB) will be used in the free
SPI flash memory determined from the partition table.

Please refer file `$RIOTBASE/tests/unittests/test-spiffs/tests-spiffs.c` for
more information on how to use SPIFFS and VFS together with a MTD device `mtd0`
alias `MTD_0`.

[Back to table of contents](#esp32_toc)

# Network Interfaces {#esp32_network_interfaces}

ESP32x SoCs integrate different network interfaces:

- **EMAC**, an Ethernet MAC implementation (requires an external PHY module)
- **ESP WiFi**, WiFi in infratructure mode (station or AP mode)
- **ESP-NOW**, WiFi based ad-hoc connectionless peer to peer communication
  protocol
- **ESP-MESH**, a WiFi based mesh technology (not yet supported)

@note EMAC interface is only available in ESP32 SoCs.

[Back to table of contents](#esp32_toc)

## Ethernet MAC Network Interface {#esp32_ethernet_network_interface}

ESP32 SoC variant (family) provides an **Ethernet MAC layer module (EMAC)**
according to the IEEE 802.3 standard which can be used together with an
external physical layer chip (PHY) to realize a 100/10 Mbps Ethernet
interface. The following PHY layer chips are supported:

- IC Plus 101G
- Microchip KSZ8041/KSZ8081
- Microchip LAN8710/LAN8720
- Realtek RTL8201
- Texas Instruments DP83848

The RIOT port for ESP32 SoCs realizes a `netdev` driver for the EMAC
(module `esp_eth`) which uses RIOT's standard Ethernet interface.

If the board has one of the supported PHY layer chips connected to the ESP32,
feature `esp_eth` should be enabled.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FEATURES_PROVIDED += periph_eth
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Furthermore, module `esp_eth` should be by default in board's `Makefile.dep`
when module `netdev_default` is used.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ifneq (,$(filter netdev_default,$(USEMODULE)))
  USEMODULE += esp_eth
endif
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Otherwise, the application has to add the `esp_eth` module in its makefile
when needed.

@note
The board has to have one of the supported PHY chips to be able to use
the Ethernet MAC module.

[Back to table of contents](#esp32_toc)

## WiFi Network Interface {#esp32_wifi_network_interface}

The RIOT port for ESP32x SoC implements a `netdev` driver for the built-in WiFi
interface. This `netdev` driver supports WPA2 personal mode as well as WPA2
enterprise mode.

[Back to table of contents](#esp32_toc)

### WPA2 personal mode

To use the WiFi `netdev` driver in WPA2 personal mode with a
preshared key (PSK), module `esp_wifi` has to be enabled.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_wifi
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Furthermore, the following configuration parameters have to be defined:

<center>

Parameter           | Default                   | Description
:-------------------|:--------------------------|:------------
#WIFI_SSID          | "RIOT_AP"                 | SSID of the AP to be used.
#WIFI_PASS          | -                         | Passphrase used for the AP as clear text (max. 64 chars).
#ESP_WIFI_STACKSIZE | #THREAD_STACKSIZE_DEFAULT | Stack size used for the WiFi netdev driver thread.

</center><br>

These configuration parameter definitions, as well as enabling the `esp_wifi`
module, can be done either in the makefile of the project or at make command
line, for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE=esp_wifi \
CFLAGS='-DWIFI_SSID=\"MySSID\" -DWIFI_PASS=\"MyPassphrase\"' \
make -C examples/networking/gnrc/gnrc_networking BOARD=...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note
- Module `esp_wifi` is not enabled automatically when module
`netdev_default` is used.
- Leave 'WIFI_PASS' undefined to connect to an open WiFi access point.
- The Wifi network interface (module `esp_wifi`) and the
[ESP-NOW network interface](#esp32_esp_now_network_interface) (module `esp_now`)
can be used simultaneously, for example, to realize a border router for
a mesh network which uses ESP-NOW.

[Back to table of contents](#esp32_toc)

### WPA2 Enterprise Mode

To use the WiFi `netdev` driver in WPA2 enterprise mode with IEEE 802.1X/EAP
authentication, module `esp_wifi_enterprise` has to be enabled.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_wifi_enterprise
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It supports the following EAP authentication methods:

- PEAPv0
- PEAPv1
- TTLS

As inner (phase 2) EAP authentication method, only MSCHAPv2 is supported.

To use module `esp_wifi_enterprise` with these authentication methods, the
following configuration parameters have to be defined:

<center>

Parameter           | Default   | Description
:-------------------|:----------|:------------
#WIFI_SSID          | "RIOT_AP" | SSID of the AP to be used.
WIFI_EAP_ID         | none      | Optional anonymous identity used in phase 1 (outer) EAP authentication.[1]
WIFI_EAP_USER       | none      | User name used in phase 2 (inner) EAP authentication.
WIFI_EAP_PASS       | none      | Password used in phase 2 (inner) EAP authentication.
#ESP_WIFI_STACKSIZE | #THREAD_STACKSIZE_DEFAULT | Stack size used for the WiFi netdev driver thread.

</center><br>

[1] If the optional anonymous identy `WIFI_EAP_ID` is not defined, the user
name `WIFI_EAP_USER` defined for phase 2 (inner) EAP authentication is used
as identity in phase 1.

These configuration parameter definitions, as well as enabling the `esp_wifi`
module, can be done either in the makefile of the project or at make command
line, for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE=esp_wifi_enterprise \
CFLAGS='-DWIFI_SSID=\"MySSID\" -DWIFI_EAP_ID=\"anonymous\" -DWIFI_EAP_USER=\"MyUserName\" -DWIFI_EAP_PASS=\"MyPassphrase\"' \
make -C examples/networking/gnrc/gnrc_networking BOARD=...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note
- Since there is no possibility to add the CA certificate to the RIOT image,
the verification of the AP certificate is not yet supported.
- Module `esp_wifi_enterprise` is not enabled automatically when module
`netdev_default` is used.
- The Wifi network interface (module `esp_wifi_enterprise`) and the
[ESP-NOW network interface](#esp32_esp_now_network_interface) (module `esp_now`)
can be used simultaneously, for example, to realize a border router for
a mesh network which uses ESP-NOW.
In this case the ESP-NOW interface must use the same channel as the AP of the
infrastructure WiFi network. All ESP-NOW nodes must therefore be compiled with
the channel of the AP as value for the parameter 'ESP_NOW_CHANNEL'.

[Back to table of contents](#esp32_toc)

## WiFi SoftAP Network Interface {#esp32_wifi_ap_network_interface}

The RIOT port for the ESP32x SoCs supports a `netdev` interface for the
ESP WiFi SoftAP mode. Module `esp_wifi_ap` has to be enabled to use it.

The following parameters can be configured:

<center>

Parameter                 | Default                   | Description
:-------------------------|:--------------------------|:-------------
#WIFI_SSID                | "RIOT_AP"                 | Static SSID definition for the SoftAP
#WIFI_PASS                | none                      | The password for the WiFi SoftAP network interface.[1]
#ESP_WIFI_SSID_DYNAMIC    | 0                         | Defines whether dynamic SSID is used for the SoftAP [2].
#ESP_WIFI_SSID_HIDDEN     | 0                         | Defines whether the SoftAP SSID should be hidden.
#ESP_WIFI_MAX_CONN        | 4                         | The maximum number of connections for the SoftAP.
#ESP_WIFI_BEACON_INTERVAL | 100                       | The beacon interval time in milliseconds for the SoftAP.
#ESP_WIFI_STACKSIZE       | #THREAD_STACKSIZE_DEFAULT | Stack size used for the WiFi netdev driver thread.

</center><br>

[1] If no password is provided, the interface will be "open", otherwise it
    uses WPA2-PSK authentication mode.<br>
[2] If `#ESP_WIFI_SSID_DYNAMIC` is set to 1, a dynamic SSID is generated for the
    SoftAP by extending the defined SSID (`WIFI_SSID`) with the MAC address
    of the SoftAP interface used, e.g.: `RIOT_AP_aabbccddeeff`

These configuration parameter definitions, as well as enabling the `esp_wifi_ap`
module, can be done either in the makefile of the project or at make command
line, for example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE=esp_wifi_ap \
CFLAGS='-DWIFI_SSID=\"MySSID\" -DWIFI_PASS=\"MyPassphrase\" -DESP_WIFI_MAX_CONN=1' \
make -C examples/networking/gnrc/gnrc_networking BOARD=...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note
- The `esp_wifi_ap` module is not used by default when `netdev_default` is used.
- Supports open and WPA2-PSK authentication modes.
- The ESP-NOW network interface and the WiFi SoftAP network interface can not
be used simultaneously.

@warning
The SoftAP may or may not be at all reliable sometimes, this is a known
problem with the Wi-Fi network interface, even on the official ESP-IDF.
The problem is that the AP doesn't cache multicast data for connected
stations, and if stations connected to the AP are power save enabled,
they may experience multicast packet loss. This affects RIOT, because
NDP relies on multicast packets to work correctly.
Refer to the SDK documentation from Espressif on
[AP Sleep](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#ap-sleep)
for more information.

[Back to table of contents](#esp32_toc)

## ESP-NOW Network Interface {#esp32_esp_now_network_interface}

With ESP-NOW, ESP32x SoCs provide a connectionless communication technology,
featuring short packet transmission. It applies the IEEE802.11 Action Vendor
frame technology, along with the IE function developed by Espressif, and CCMP
encryption technology, realizing a secure, connectionless communication
solution.

The RIOT port for ESP32x SoCs implements in module `esp_now` a `netdev`
driver which uses ESP-NOW to provide a link layer interface to a meshed network
of ESP32x nodes. In this network, each node can send short packets with up to
250 data bytes to all other nodes that are visible in its range.

@note Module `esp_now`module is not enabled automatically if the
`netdev_default` module is used. Instead, the application has to add the
`esp_now` module in its makefile when needed.<br>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += esp_now
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For ESP-NOW, ESP32x nodes are used in WiFi SoftAP + Station mode to advertise
their SSID and become visible to other ESP32x nodes. The SSID of an ESP32x node
is the concatenation of the prefix `RIOT_ESP_` with the MAC address of its
SoftAP WiFi interface. The driver periodically scans all visible ESP32x nodes.

The following parameters are defined for ESP-NOW nodes. These parameters can
be overridden by
[application-specific board configurations](#esp32_application_specific_board_configuration).

<center>

Parameter               | Default                   | Description
:-----------------------|:--------------------------|:-----------
ESP_NOW_SCAN_PERIOD_MS  | 10000UL                   | Defines the period in ms at which an node scans for other nodes in its range. The default period is 10 s.
ESP_NOW_SOFT_AP_PASS    | "ThisistheRIOTporttoESP"  | Defines the passphrase as clear text (max. 64 chars) that is used for the SoftAP interface of ESP-NOW nodes. It has to be same for all nodes in one network.
ESP_NOW_CHANNEL         | 6                         | Defines the channel that is used as the broadcast medium by all nodes together.
ESP_NOW_KEY             | NULL                      | Defines a key that is used for encrypted communication between nodes. If it is NULL, encryption is disabled. The key has to be of type `uint8_t[16]` and has to be exactly 16 bytes long.

</center><br>

@note The ESP-NOW network interface (module `esp_now`) and the
[Wifi network interface](#esp32_wifi_network_interface) (module `esp_wifi` or
`esp_wifi_enterprise`) can be used simultaneously, for example, to realize a
border router for a mesh network which uses ESP-NOW.
In this case the ESP-NOW interface must use the same channel as the AP of the
infrastructure WiFi network. All ESP-NOW nodes must therefore be compiled with
the channel of the AP asvalue for the parameter 'ESP_NOW_CHANNEL'.

[Back to table of contents](#esp32_toc)

## Bluetooth Network Interface {#esp32_esp_bluetooth_interface}

The following ESP32x SoC variants (families) integrate a Bluetooth Link
Controller and a Bluetooth baseband system:

- ESP32 supports Bluetooth v4.2 BR/EDR and Bluetooth LE
- ESP32-C3, ESP32-H2, ESP32-S3 support Bluetooth 5 and Bluetooth mesh

The Bluetooth interface can be used with the Bluetooth host implementation
of the NimBLE package. Use one of the `nimble_*` modules for different
applications to enable the Bluetooth interface and the NimBLE host
implementation. Please refer to the NimBle package documentation for details.

[Back to table of contents](#esp32_toc)

## IEEE 802.15.4 Network Interface {#esp32_esp_ieee802154_interface}

ESP-H2 includes an IEEE 802.15.4 subsystem
that integrates PHY and MAC layer.

The \ref drivers_ieee802154_hal "IEEE 802.15.4 Hardware Abstraction Layer"
driver for the IEEE 802.15.4 subsystem of ESP32x SoCs (module `esp_ieee802154`)
can be used together with RIOT's \ref net_ieee802154_submac "IEEE 802.15.4 SubMAC layer"
to use the IEEE 802.15.4 subsystem of ESP32x SoCs as network interface.

[Back to table of contents](#esp32_toc)

## Other Network Devices {#esp32_other_network_devices}

RIOT provides a number of driver modules for different types of network
devices, e.g., IEEE 802.15.4 radio modules and Ethernet modules. The RIOT port
for ESP32x SoCs has been tested with the following network devices:

- \ref drivers_mrf24j40 "mrf24j40" (driver for Microchip MRF24j40 based IEEE 802.15.4)
- \ref drivers_enc28j60 "enc28j60" (driver for Microchip ENC28J60 based Ethernet modules)

[Back to table of contents](#esp32_toc)

### Using MRF24J40 (module mrf24j40) {#esp32_using_mrf24j40}

To use MRF24J40 based IEEE 802.15.4 modules as network device, the
`mrf24j40` driver module has to be added to the makefile of the
application:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += mrf24j40
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The driver parameters that have to be defined by the board configuration for
the MRF24J40 driver module are:

Parameter              | Description
-----------------------|------------
MRF24J40_PARAM_CS      | GPIO used as CS signal
MRF24J40_PARAM_INT     | GPIO used as interrupt signal
MRF24J40_PARAM_RESET   | GPIO used as reset signal

Since each board has different GPIO configurations, refer to the board
documentation for the GPIOs recommended for the MRF24J40.

@note The reset signal of the MRF24J40 based network device can be connected
with the ESP32x RESET/EN pin  which is broken out on most boards. This keeps the
GPIO free defined by `MRF24J40_PARAM_RESET` free for other purposes.

[Back to table of contents](#esp32_toc)

### Using ENC28J60 (module enc28j60) {#esp32_using_enc28j60}

To use ENC28J60 Ethernet modules as network device, the `enc28j60` driver
module has to be added to the makefile of the application:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
USEMODULE += enc28j60
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The parameters that have to be defined by board configuration for the ENC28J60
driver module are:

Parameter            | Description
---------------------|------------
ENC28J60_PARAM_CS    | GPIO used as CS signal
ENC28J60_PARAM_INT   | GPIO used as interrupt signal
ENC28J60_PARAM_RESET | GPIO used as reset signal

Since each board has different GPIO configurations, refer to the board
documentation for the GPIOs recommended for the ENC28J60.

@note The reset signal of the ENC28J60 based network device can be connected
with the ESP32x RESET/EN pin  which is broken out on most boards. This keeps the
GPIO free defined by `#ENC28J60_PARAM_RESET` free for other purposes.

[Back to table of contents](#esp32_toc)

# Application-Specific Configurations {#esp32_application_specific_configurations}

The board-specific configuration files `board.h` and `periph_conf.h` as
well well as the driver parameter configuration files `<driver>_params.h`
define the default configurations for peripherals and device driver modules.
These are, for example, the GPIOs used, bus interfaces used or available bus
speeds. Because there are many possible configurations and many different
application requirements, these default configurations are usually only a
compromise between different requirements.

Therefore, it is often necessary to change some of these default configurations
for individual applications. For example, while many PWM channels are needed in
one application, another application does not need PWM channels, but many ADC
channels.

There are two ways to give the application the ability to change some of these
default configurations:

- make variable `CFLAGS`
- application-specific board or driver configuration file

[Back to table of contents](#esp32_toc)

## Make Variable CFLAGS {#esp32_config_make_variable}

Using the `CFLAGS` make variable at the command line, board or driver parameter
definitions can be overridden.

Example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS='-DESP_LCD_PLUGGED_IN=1 -DLIS3DH_PARAM_INT2=GPIO4'
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When a larger number of board definitions needs be overridden, this approach
becomes impractical. In that case, an application-specific board configuration
file located in application directory can be used, see sections below.

[Back to table of contents](#esp32_toc)

## Application-Specific Board Configuration {#esp32_application_specific_board_configuration}

To override default board configurations, simply create an application-specific
board configuration file `$APPDIR/board.h` in the source directory
`$APPDIR` of the application and add the definitions to be overridden. To
force the preprocessor to include board's original `board.h` after that,
add the `include_next` preprocessor directive as the **last** line.

For example to override the default definition of the GPIOs that are used as
PWM channels, the application-specific board configuration file
`$APPDIR/board.h` could look like the following:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef CPU_ESP32
#define PWM0_CHANNEL_GPIOS { GPIO12, GPIO13, GPIO14, GPIO15 }
#endif

#include_next "board.h"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is important that the application-specific board
configuration `$APPDIR/board.h` is included first. Insert the following
line as the **first** line to the application makefile `$APPDIR/Makefile`.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INCLUDES += -I$(APPDIR)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note To make such application-specific board configurations dependent on a
      certain ESP32x SoC variant (family) or a particular ESP32x board, you
      should always enclose these definitions in the following constructs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef CPU_FAM_ESP32            // specific ESP32x SoC variant (family)
...
#endif

#ifdef BOARD_ESP32_WROVER_KIT   // specific ESP32x board
...
#endif
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp32_toc)

## Application-Specific Driver Configuration {#esp32_application_specific_driver_configuration}

Using the approach for overriding board configurations, the parameters of
drivers that are typically defined in
`drivers/<device>/include/<device>_params.h` can be overridden. For that
purpose just create an application-specific driver parameter file
`$APPDIR/<device>_params.h` in the source directory `$APPDIR` of the
application and add the definitions to be overridden. To force the preprocessor
to include driver's original `<device>_params.h` after that, add the
`include_next` preprocessor directive as the **last** line.

For example, to override a GPIO used for LIS3DH sensor, the
application-specific driver parameter file `$APPDIR/<device>_params.h`
could look like the following:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef CPU_FAM_ESP32
#define LIS3DH_PARAM_INT2           (GPIO_PIN(0, 4))
#endif

#include_next "lis3dh_params.h"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is important to ensure that the application-specific driver parameter file
`$APPDIR/<device>_params.h` is included first. Insert the following line as
the *first* line to the application makefile `$APPDIR/Makefile`.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INCLUDES += -I$(APPDIR)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

@note To make such application-specific board configurations dependent on a
      certain ESP32x SoC variant (family) or a particular ESP32x board, you
      should always enclose these definitions in the following constructs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
#ifdef CPU_FAM_ESP32            // specific ESP32x SoC variant (family)
...
#endif

#ifdef BOARD_ESP32_WROVER_KIT   // specific ESP32x board
...
#endif
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp32_toc)

# Debugging {#esp32_debugging}

## JTAG Debugging {#esp32_jtag_debugging}

ESP32x SoCs integrate a JTAG interface for On-Chip Debugging. The GPIOs
connected to this JTAG interface depend on the ESP32x SoC variant (family).
For details, see:

- \ref esp32_jtag_interface_esp32 "ESP32"
- \ref esp32_jtag_interface_esp32c3 "ESP32-C3"
- \ref esp32_jtag_interface_esp32h2 "ESP32-H2"
- \ref esp32_jtag_interface_esp32s2 "ESP32-S2"
- \ref esp32_jtag_interface_esp32s3 "ESP32-S3"

This JTAG interface can be used with OpenOCD and GDB for On-Chip debugging
of your software on instruction level. When you compile your software with
debugging information (module `esp_gdb`) you can debug on source code
level as well.

@note
When debugging, the GPIOs used for the JTAG interface must not be used for
anything else.

Some ESP32 boards like the \ref esp32_wrover_kit_toc "ESP-WROVER-KIT V3" or the
\ref esp32-ethernet-kit-v1_0 "ESP32-Ethernet-Kit " have a USB bridge with
JTAG interface on-board that can be directly used for JTAG debugging.

Other ESP32x SoC variants (families) have an built in USB-to-JTAG bridge
that can be used without additional chips. For details, see:

- \ref esp32_jtag_interface_esp32 "ESP32"
- \ref esp32_jtag_interface_esp32c3 "ESP32-C3"
- \ref esp32_jtag_interface_esp32h2 "ESP32-H2"
- \ref esp32_jtag_interface_esp32s2 "ESP32-S2"
- \ref esp32_jtag_interface_esp32s3 "ESP32-S3"

To use the JTAG debugging, the precompiled version of OpenOCD for ESP32 has to
be installed using the toolchain install script while being in RIOT's root
directory, see also section
[Using Local Toolchain Installation](#esp32_local_toolchain_installation).
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ dist/tools/esptools/install.sh openocd
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Before OpenOCD can be used, the `PATH` variable has to be set correctly
and the `OPENOCD` variable has to be exported using the following command.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ . dist/tools/esptools/export.sh openocd
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once the `PATH` variable as well as the `OPENOCD` variable are set, the
debugging session can be started using either
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ PROGRAMMER=openocd USEMODULE=esp_jtag \
  make debug BOARD=esp32-wrover-kit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if the board defines an OpenOCD board configuration file or using
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ PROGRAMMER=openocd USEMODULE=esp_jtag OPENOCD_CONFIG=board/esp-wroom-32.cfg \
  make debug BOARD=esp32-wroom-32
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if the board does not define an OpenOCD board configuration file.

@note
The board will be reset, but not flashed with this command. However, flashing
would be also possible with OpenOCD and the JTAG interface using command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
$ PROGRAMMER=openocd USEMODULE=esp_jtag \
  make flash BOARD=esp32-wrover-kit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Detailed information on how to configure the JTAG interface of the respective
ESP32x SoC variant (family) can be found in ESP-IDF Programming Guide:

- [ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/jtag-debugging/index.html)
- [ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/jtag-debugging/index.html)
- [ESP32-H2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32h2/api-guides/jtag-debugging/index.html)
- [ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/jtag-debugging/index.html)
- [ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/jtag-debugging/index.html)
- [ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/jtag-debugging/index.html)

[Back to table of contents](#esp32_toc)

## QEMU Mode and GDB {#esp32_qemu_mode_and_gdb}

RIOT applications that do not require interaction with real hardware such as
GPIOs, I2C or SPI devices, WiFi interface, etc. can also be debugged using
QEMU. For this purpose, either QEMU for ESP32x SoCs must be installed,
see section [Local Toolchain Installation](#esp32_local_toolchain_installation)
or the RIOT Docker build image has to be used in which QEMU for ESP32x SoCs is already
installed.

To use QEMU for ESP32x SoCs, an application has to be built with `esp_qemu` module
enabled, for example with local toolchain installation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.bash
$ USEMODULE=esp_qemu make flash BOARD=esp32-wroom-32 -C tests/sys/shell/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
or with RIOT Docker build image
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.bash
$ BUILD_IN_DOCKER=1 DOCKER="sudo docker" \
  USEMODULE=esp_qemu make flash BOARD=esp32-wroom-32 -C tests/sys/shell/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Instead of flashing the image to the target hardware, a binary image named
`qemu_flash_image.bin` is created in the target directory. In addition, two ROM
files `rom.bin` and `rom1.bin` are copied to the target directory. These
files can then be used with QEMU for ESP32x SoCs to debug the application in GDB
without having the hardware. The binary image `qemu_flash_image.bin`
represents a 4 MByte Flash image.

To start QEMU for ESP32x SoCs, the following command has to be used.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.bash
$ qemu-system-<arch> \
    -s -machine <esp32x_variant> \
    -display none \
    -drive file=<path_to_image>/qemu_flash_image.bin,if=mtd,format=raw \
    -serial tcp::5555,server,nowait
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
with

- `arch` = `xtensa` | `riscv32` and
- `esp32x_variant` = `esp32` | `esp32s3` | `esp32c3`.

For example, for an ESP32, QEMU can be started with command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.bash
$ qemu-system-xtensa \
    -s -machine esp32 \
    -display none \
    -drive file=tests/sys/shell/bin/esp32-wroom-32/qemu_flash_image.bin,if=mtd,format=raw \
    -serial tcp::5555,server,nowait
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To interact with the application on the emulated ESP32x SoC in QEMU, a second
terminal is required in which the `telnet` command is used to communicate
with the application on `localhost` using TCP port 5555:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.bash
$ telnet localhost 5555

Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.

main(): This is RIOT! (Version: 2022.04)
test_shell.

> help
help
Command              Description
---------------------------------------
bufsize              Get the shell's buffer size
start_test           starts a test
...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To debug the application in QEMU for a ESP32x SoC, another terminal is required
in which the corresponding GDB hast to be started:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.bash
$ xtensa-esp32-elf-gdb tests/sys/shell//bin/esp32-wroom-32/tests_shell.elf

GNU gdb (crosstool-NG crosstool-ng-1.22.0-80-g6c4433a5) 7.10
Copyright (C) 2015 Free Software Foundation, Inc.
...
Reading symbols from tests/sys/shell//bin/esp32-wroom-32/tests_shell.elf...done.
(gdb) target remote :1234

Remote debugging using :1234
pm_set (mode=2) at cpu/esp32/periph/pm.c:117
117	        return;
(gdb)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QEMU for ESP32x SoCs can also be used in RIOT Docker build image. For that purpose
QEMU has to be started in the Docker container, for example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.bash
$ sudo docker run --rm -i -t -u $UID -v $(pwd):/data/riotbuild riot/riotbuild
riotbuild@container-id:~$ USEMODULE=esp_qemu make flash BOARD=esp32-wroom-32 -C tests/sys/shell/
riotbuild@container-id:~$ qemu-system-xtensa \
    -s -machine esp32 \
    -display none \
    -drive file=tests/sys/shell/bin/esp32-wroom-32/qemu_flash_image.bin,if=mtd,format=raw \
    -serial tcp::5555,server,nowait
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In a second and a third terminal, you need to execute a shell in the same RIOT
Docker container where QEMU was started. The required container ID
`<container-id>` is shown in the prompt of the terminal in which QEMU
was started.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.bash
$ sudo docker docker exec -it <container-id> bash
riotbuild@container-id:~$telnet localhost 5555

Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.

main(): This is RIOT! (Version: 2022.04)
test_shell.
>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.bash
$ sudo docker docker exec -it <container-id> bash
riotbuild@container-id:~$ xtensa-esp32-elf-gdb tests/sys/shell//bin/esp32-wroom-32/tests_shell.elf

GNU gdb (crosstool-NG crosstool-ng-1.22.0-80-g6c4433a5) 7.10
Copyright (C) 2015 Free Software Foundation, Inc.
...
Reading symbols from tests/sys/shell//bin/esp32-wroom-32/tests_shell.elf...done.
(gdb) target remote :1234

Remote debugging using :1234
pm_set (mode=2) at cpu/esp32/periph/pm.c:117
117	        return;
(gdb)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[Back to table of contents](#esp32_toc)
