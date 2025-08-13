/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       ESP32-C3 specific peripheral configuration
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Mapping configured ESP32-C3 default clock to CLOCK_CORECLOCK define */
#define CLOCK_CORECLOCK     (1000000UL * CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

/**
 * @brief   CPU cycles per busy wait loop
 */
#define CPU_CYCLES_PER_LOOP (4)

/**
 * @name   Predefined GPIO names
 * @{
 */
#define GPIO0       (GPIO_PIN(PORT_GPIO, 0))
#define GPIO1       (GPIO_PIN(PORT_GPIO, 1))
#define GPIO2       (GPIO_PIN(PORT_GPIO, 2))
#define GPIO3       (GPIO_PIN(PORT_GPIO, 3))
#define GPIO4       (GPIO_PIN(PORT_GPIO, 4))
#define GPIO5       (GPIO_PIN(PORT_GPIO, 5))
#define GPIO6       (GPIO_PIN(PORT_GPIO, 6))
#define GPIO7       (GPIO_PIN(PORT_GPIO, 7))
#define GPIO8       (GPIO_PIN(PORT_GPIO, 8))
#define GPIO9       (GPIO_PIN(PORT_GPIO, 9))
#define GPIO10      (GPIO_PIN(PORT_GPIO, 10))
#define GPIO11      (GPIO_PIN(PORT_GPIO, 11))
#define GPIO12      (GPIO_PIN(PORT_GPIO, 12))
#define GPIO13      (GPIO_PIN(PORT_GPIO, 13))
#define GPIO14      (GPIO_PIN(PORT_GPIO, 14))
#define GPIO15      (GPIO_PIN(PORT_GPIO, 15))
#define GPIO16      (GPIO_PIN(PORT_GPIO, 16))
#define GPIO17      (GPIO_PIN(PORT_GPIO, 17))
#define GPIO18      (GPIO_PIN(PORT_GPIO, 18))
#define GPIO19      (GPIO_PIN(PORT_GPIO, 19))
#define GPIO20      (GPIO_PIN(PORT_GPIO, 20))
#define GPIO21      (GPIO_PIN(PORT_GPIO, 21))
/** @} */

/**
 * @name   ADC configuration
 *
 * ESP32-C3 integrates two 12-bit ADCs (ADC1 and ADC2) with 6 channels in
 * total:
 *
 * - **ADC1** supports 5 channels: GPIO0, GPIO1, GPIO2, GPIO3 and GPIO4
 * - **ADC2** supports 1 channel: GPIO5 or internal signals such as vdd33
 *
 * The maximum number of ADC channels #ADC_NUMOF_MAX is 6.
 *
 * @note
 *   ADC2 is also used by the WiFi module. The GPIOs connected to ADC2 are
 *   therefore not available as ADC channels if the modules `esp_wifi` or
 *   `esp_now` are used.
 */

/**
 * @name   I2C configuration
 *
 * ESP32-C3 has one built-in I2C interfaces.
 *
 * The following table shows the default configuration of I2C interfaces
 * used for ESP32-C3 boards. It can be overridden by
 * [application-specific configurations](#esp32_application_specific_configurations).
 *
 * <center>
 *
 * Device     | Signal | Pin    | Symbol        | Remarks
 * :----------|:-------|:-------|:--------------|:----------------
 * I2C_DEV(0) |        |        | `#I2C0_SPEED` | default is `I2C_SPEED_FAST`
 * I2C_DEV(0) | SCL    | GPIO4  | `#I2C0_SCL`   | -
 * I2C_DEV(0) | SDA    | GPIO5  | `#I2C0_SDA`   | -
 *
 * </center><br>
 */

/**
 * @name   PWM configuration
 *
 * The ESP32-C3 LEDC module has 1 channel group with 6 channels. Each of
 * these channels can be clocked by one of the 4 timers.
 */

/**
 * @name   SPI configuration
 *
 * ESP32-C3 has three SPI controllers where SPI0 and SPI1 share the same bus
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
 * :-----------------------|:------:|:-------|:-----------:|:---------------------------
 * `SPI_HOST0`/`SPI_HOST1` | SPICS0 | GPIO14 | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPICLK | GPIO15 | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPID   | GPIO16 | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPIQ   | GPIO17 | - | reserved for Flash and PSRAM
 * `SPI_HOST0`/`SPI_HOST1` | SPIHD  | GPIO12 | - | reserved for Flash and PSRAM (only in `qio` or `qout` mode)
 * `SPI_HOST0`/`SPI_HOST1` | SPIWP  | GPIO13 | - | reserved for Flash and PSRAM (only in `qio` or `qout` mode)
 * `SPI_HOST2` (`FSPI`)    | SCK    | GPIO6  |`#SPI0_SCK`  | can be used
 * `SPI_HOST2` (`FSPI`)    | MOSI   | GPIO7  |`#SPI0_MOSI` | can be used
 * `SPI_HOST2` (`FSPI`)    | MISO   | GPIO2  |`#SPI0_MISO` | can be used
 * `SPI_HOST2` (`FSPI`)    | CS0    | GPIO10 |`#SPI0_CS0`  | can be used
 *
 * </center><br>
 */

/**
 * @name   Timer configuration depending on which implementation is used
 *
 * ESP32-C3 has two timer groups with one channel each.
 */

#ifdef MODULE_ESP_HW_COUNTER
#error "Counter based timers are not supported by ESP32-C3"
#endif

/**
 * @name   UART configuration
 *
 * ESP32-C3 integrates two UART interfaces. The following default pin
 * configuration of UART interfaces as used by a most boards can be overridden
 * by the application, see section [Application-Specific Configurations]
 * (#esp32_application_specific_configurations).
 *
 * <center>
 *
 * Device      |Signal|Pin     |Symbol      |Remarks
 * :-----------|:-----|:-------|:-----------|:----------------
 * UART_DEV(0) | TxD  | GPIO21 |`#UART0_TXD`| cannot be changed
 * UART_DEV(0) | RxD  | GPIO20 |`#UART0_RXD`| cannot be changed
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
