/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32c3_devkit
 * @brief       Peripheral configurations for ESP32-C3-DevKit boards
 * @{
 *
 * The peripheral configurations in this file are valid for Espressif
 * ESP32-C3-DevKitx boards that use one of the following modules:
 *
 * - ESP32-C3-MINI-1x module (ESP32-C3-DevKitM-1 board)
 * - ESP32-C3-WROOM-02x module (ESP32-C3-DevKitC-02 board)
 *
 * Since the number of GPIOs and their possible uses on the ESP32-C3 are quite
 * limited, these peripheral configurations can also be used for most other
 * ESP32-C3 boards. Any modifications required for specific applications
 * can be overridden by \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * For detailed information about the peripheral configuration for ESP32-C3
 * boards, see section \ref esp32_peripherals "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_application_specific_configurations
 * "application-specific board configuration" if necessary.
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
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
 * For generic boards, all ADC pins that have broken out are declared as ADC
 * channels.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#define ADC_GPIOS   { GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5 }
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * For generic boards, only one I2C interface I2C_DEV(0) is defined.
 *
 * The GPIOs listed in the configuration are only initialized as I2C signals
 * when module `periph_i2c` is used. Otherwise they are not allocated and
 * can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#define I2C0_SPEED  I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#define I2C0_SCL    GPIO4           /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO5           /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * For generic boards, two PWM devices are configured. These devices
 * contain all GPIOs that are not defined as I2C or UART for this board.
 * Generally, all outputs pins could be used as PWM channels.
 *
 * @note As long as the according PWM device is not initialized with
 * the `pwm_init`, the GPIOs declared for this device can be used
 * for other purposes.
 *
 * @{
 */

/**
 * @brief Declaration of the channels for device PWM_DEV(0),
 *        at maximum PWM_CHANNEL_NUM_DEV_MAX.
 */
#ifndef PWM0_GPIOS
#  define PWM0_GPIOS  { GPIO3, GPIO1 }
#endif

/**
 * @brief Declaration of the channels for device PWM_DEV(1),
 *        at maximum PWM_CHANNEL_NUM_DEV_MAX.
 *
 * PWM_DEV(1) is only used if SPI peripheral is not enabled.
 */
#if !defined(PWM1_GPIOS) && !defined(MODULE_PERIPH_SPI)
#  define PWM1_GPIOS  { GPIO10, GPIO7 }
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
 * @{
 */
#ifndef SPI0_CTRL
#define SPI0_CTRL   FSPI    /**< FSPI is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#define SPI0_SCK    GPIO6   /**< FSPI SCK (pin FSPICLK) */
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO2   /**< FSPI MISO (pin FSPIQ) */
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO7   /**< FSPI MOSI (pin FSPID) */
#endif
#ifndef SPI0_CS0
#define SPI0_CS0    GPIO10  /**< FSPI CS0 (pin FSPICS0) */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32-C3 provides 2 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is not used.<br>
 *
 * @{
 */
#define UART0_TXD   GPIO21  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO20  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
