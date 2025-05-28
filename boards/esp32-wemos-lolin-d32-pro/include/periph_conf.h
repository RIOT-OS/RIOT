/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32_wemos-lolin-d32-pro
 * @brief       Peripheral MCU configuration for Wemos LOLIN D32 Pro
 * @{
 *
 *
 * Wemos LOLIN D32 Pro is a development board that uses the ESP32-WROVER
 * module which has a built-in 4 MByte SPI RAM. Most important
 * features of the board are
 *
 * - Micro-SD card interface
 * - LCD interface
 * - SPI RAM 4 MByte
 *
 * Furthermore, most GPIOs are broken out for extension.
 *
 * When the TFT display is connected, add
 * ```
 * USEMODULE += esp_lolin_tft
 * ```
 * to the makefile of the application to use the according default board
 * configuration.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    ADC and DAC channel configuration
 * @{
 */

/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * GPIO35 is used to measure V_BAT and is therefore not broken out.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#ifndef MODULE_ESP_LOLIN_TFT
#define ADC_GPIOS   { GPIO36, GPIO39, GPIO34, GPIO35, GPIO32, GPIO33 }
#else
#define ADC_GPIOS   { GPIO36, GPIO39, GPIO34, GPIO35 }
#endif
#endif

/**
 * @brief   Declaration of GPIOs that can be used as DAC channels
 *
 * @note As long as the GPIOs listed in DAC_GPIOS are not initialized as DAC
 * channels with the `dac_init` function, they can be used for other
 * purposes.
 */
#ifndef DAC_GPIOS
#define DAC_GPIOS   { GPIO25, GPIO26 }
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * Only I2C interface I2C_DEV(0) is used.
 *
 * @note The GPIOs listed in the configuration are only initialized as I2C
 * signals when module `periph_i2c` is used. Otherwise they are not
 * allocated and can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#define I2C0_SPEED  I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#define I2C0_SCL    GPIO22          /**< SCL signal of I2C_DEV(0) [UEXT1] */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO21          /**< SDA signal of I2C_DEV(0) [UEXT1] */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * @note As long as the according PWM device is not initialized with
 * the `pwm_init`, the GPIOs declared for this device can be used
 * for other purposes.
 *
 * @{
 */
/** PWM channels for device PWM_DEV(0) */
#ifndef PWM0_GPIOS
#define PWM0_GPIOS { GPIO0, GPIO2 }
#endif

/** @} */

/**
 * @name    SPI configuration
 *
 * @note The GPIOs listed in the configuration are first initialized as SPI
 * signals when the corresponding SPI interface is used for the first time
 * by either calling the `spi_init_cs` function or the `spi_acquire`
 * function. That is, they are not allocated as SPI signals before and can
 * be used for other purposes as long as the SPI interface is not used.
 *
 * @{
 */
#ifndef SPI0_CTRL
#define SPI0_CTRL   VSPI    /**< VSPI is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#define SPI0_SCK    GPIO18  /**< VSPI SCK */
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO19  /**< VSPI MISO */
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO23  /**< VSPI MOSI */
#endif
#ifndef SPI0_CS0
#define SPI0_CS0    GPIO5   /**< VSPI CS0 */
#endif

#ifdef MODULE_SDCARD_SPI
#define SPI0_CS1    GPIO4   /**< VSPI CS1 / SD card [TF_CS] */
#endif

#ifdef MODULE_ESP_LOLIN_TFT
#define SPI0_CS2    GPI14   /**< VSPI CS2 / TFT [TFT_CS]    */
#define SPI0_CS3    GPI12   /**< VSPI CS3 / TFT [TS_CS]     */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32 provides 3 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is not used.<br>
 * UART_DEV(2) is not used.<br>
 *
 * @{
 */
#define UART0_TXD   GPIO1  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO3  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "periph_conf_common.h"

/** @} */
