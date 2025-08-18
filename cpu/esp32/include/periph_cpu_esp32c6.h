/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       ESP32-C6 specific peripheral configuration
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Mapping configured ESP32-C6 default clock to CLOCK_CORECLOCK define */
#define CLOCK_CORECLOCK     (1000000UL * CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

/**
 * @brief   CPU cycles per busy wait loop
 */
#define CPU_CYCLES_PER_LOOP (4)

/**
 * @name   Predefined GPIO names
 * @{
 */
#define GPIO0  (GPIO_PIN(PORT_GPIO, 0))  /**< GPIO0 */
#define GPIO1  (GPIO_PIN(PORT_GPIO, 1))  /**< GPIO1 */
#define GPIO2  (GPIO_PIN(PORT_GPIO, 2))  /**< GPIO2 */
#define GPIO3  (GPIO_PIN(PORT_GPIO, 3))  /**< GPIO3 */
#define GPIO4  (GPIO_PIN(PORT_GPIO, 4))  /**< GPIO4 */
#define GPIO5  (GPIO_PIN(PORT_GPIO, 5))  /**< GPIO5 */
#define GPIO6  (GPIO_PIN(PORT_GPIO, 6))  /**< GPIO6 */
#define GPIO7  (GPIO_PIN(PORT_GPIO, 7))  /**< GPIO7 */
#define GPIO8  (GPIO_PIN(PORT_GPIO, 8))  /**< GPIO8 */
#define GPIO9  (GPIO_PIN(PORT_GPIO, 9))  /**< GPIO9 */
#define GPIO10 (GPIO_PIN(PORT_GPIO, 10)) /**< GPIO10 is not broken out on chips with flash */
#define GPIO11 (GPIO_PIN(PORT_GPIO, 11)) /**< GPIO11 is not broken out on chips with flash */
#define GPIO12 (GPIO_PIN(PORT_GPIO, 12)) /**< GPIO12 */
#define GPIO13 (GPIO_PIN(PORT_GPIO, 13)) /**< GPIO13 */
#define GPIO14 (GPIO_PIN(PORT_GPIO, 14)) /**< GPIO14 is not usable for chips w/o flash */
#define GPIO15 (GPIO_PIN(PORT_GPIO, 15)) /**< GPIO15 */
#define GPIO16 (GPIO_PIN(PORT_GPIO, 16)) /**< GPIO16 */
#define GPIO17 (GPIO_PIN(PORT_GPIO, 17)) /**< GPIO17 */
#define GPIO18 (GPIO_PIN(PORT_GPIO, 18)) /**< GPIO18 */
#define GPIO19 (GPIO_PIN(PORT_GPIO, 19)) /**< GPIO19 */
#define GPIO20 (GPIO_PIN(PORT_GPIO, 20)) /**< GPIO20 */
#define GPIO21 (GPIO_PIN(PORT_GPIO, 21)) /**< GPIO21 */
#define GPIO22 (GPIO_PIN(PORT_GPIO, 22)) /**< GPIO22 */
#define GPIO23 (GPIO_PIN(PORT_GPIO, 23)) /**< GPIO23 */
#define GPIO24 (GPIO_PIN(PORT_GPIO, 24)) /**< GPIO24 */
#define GPIO25 (GPIO_PIN(PORT_GPIO, 25)) /**< GPIO25 is not broken out on chips with flash */
#define GPIO26 (GPIO_PIN(PORT_GPIO, 26)) /**< GPIO26 is not broken out on chips with flash */
#define GPIO27 (GPIO_PIN(PORT_GPIO, 27)) /**< GPIO27 is not broken out on chips with flash */
#define GPIO28 (GPIO_PIN(PORT_GPIO, 28)) /**< GPIO28 is not broken out on chips with flash */
#define GPIO29 (GPIO_PIN(PORT_GPIO, 29)) /**< GPIO29 is not broken out on chips with flash */
#define GPIO30 (GPIO_PIN(PORT_GPIO, 30)) /**< GPIO30 is not broken out on chips with flash */
/** @} */

/**
 * @name   ADC configuration
 *
 * ESP32-C6 integrates one 12-bit ADC (ADC1) with 7 channels:
 *
 * GPIO0, GPIO01, GPIO2, GPIO3, GPIO4, GPIO5 and GPIO6
 *
 * The maximum number of ADC channels #ADC_NUMOF_MAX is 7.
 */

/**
 * @name   I2C configuration
 *
 * ESP32-C6 has one built-in I2C interfaces.
 *
 * The following table shows the default configuration of I2C interfaces
 * used for ESP32-C6 boards. It can be overridden by
 * [application-specific configurations](#esp32_application_specific_configurations).
 *
 * <center>
 *
 * Device     | Signal | Pin    | Symbol        | Remarks
 * :----------|:-------|:-------|:--------------|:----------------
 * I2C_DEV(0) |        |        | `#I2C0_SPEED` | default is `I2C_SPEED_FAST`
 * I2C_DEV(0) | SCL    | GPIO5  | `#I2C0_SCL`   | -
 * I2C_DEV(0) | SDA    | GPIO4  | `#I2C0_SDA`   | -
 *
 * </center><br>
 */

/**
 * @name   PWM configuration
 *
 * The ESP32-C6 LEDC module has 1 channel group with 6 channels. Each of
 * these channels can be clocked by one of the 4 timers. This means that
 * it is possible to define up to 4 PWM devices with different frequencies
 * and resolutions and a total of 6 PWM channels.
 */

/**
 * @name   SPI configuration
 *
 * ESP32-C6 has three SPI controllers where SPI0 and SPI1 share the same bus
 * and can only operate in memory mode while SPI2 can be used as general
 * purpose SPI:
 *
 * - controller SPI0 is reserved for external memories like flash and PSRAM
 * - controller SPI1 is reserved for external memories like flash and PSRAM
 * - controller SPI2 can be used for peripherals (also called FSPI)
 *
 * Thus, only SPI2 (FSPI) can be used as general purpose SPI in RIOT as
 * SPI_DEV(0).
 *
 * The following table shows the pin configuration used by default, even
 * though it **can vary** from board to board.
 *
 * <center>
 *
 * Device (Alias)          | Signal | Pin    | Symbol      | Remarks
 * :-----------------------|:------:|:-------|:-----------:|:------------------
 * `SPI_HOST0`/`SPI_HOST1` | SPICS0 | - | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPICLK | - | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPID   | - | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPIQ   | - | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPIHD  | - | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPIWP  | - | - | reserved for Flash and PSRAM
 * `SPI_HOST2` (`FSPI`)    | SCK    | GPIO6  |`#SPI0_SCK`  | can be used
 * `SPI_HOST2` (`FSPI`)    | MOSI   | GPIO7  |`#SPI0_MOSI` | can be used
 * `SPI_HOST2` (`FSPI`)    | MISO   | GPIO2  |`#SPI0_MISO` | can be used
 * `SPI_HOST2` (`FSPI`)    | CS0    | GPIO18 |`#SPI0_CS0`  | can be used
 *
 * </center><br>
 */

/**
 * @name   Timer configuration depending on which implementation is used
 *
 * ESP32-C6 has two timer groups with one channel each.
 */

#ifdef MODULE_ESP_HW_COUNTER
#  error "Counter based timers are not supported by ESP32-C6"
#endif

/**
 * @name   UART configuration
 *
 * ESP32-C6 integrates three UART interfaces, two in the high-power domain and
 * one in the low-power domain. The low-power UART interface is not yet
 * supported. The following default pin configuration of UART interfaces as
 * used by a most
 * boards can be overridden by the application, see section
 * [Application-Specific Configurations](#esp32_application_specific_configurations).
 *
 * <center>
 *
 * Device      |Signal|Pin     |Symbol      |Remarks
 * :-----------|:-----|:-------|:-----------|:----------------
 * UART_DEV(0) | TxD  | GPIO16 |`#UART0_TXD`| cannot be changed
 * UART_DEV(0) | RxD  | GPIO17 |`#UART0_RXD`| cannot be changed
 * UART_DEV(1) | TxD  | -      |`#UART1_TXD`| optional, can be overridden (no direct I/O)
 * UART_DEV(1) | RxD  | -      |`#UART1_RXD`| optional, can be overridden (no direct I/O)
 *
 * </center><br>
 *
 */

#ifdef __cplusplus
}
#endif

/** @} */
