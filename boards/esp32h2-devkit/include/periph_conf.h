/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32h2_devkit
 * @brief       Peripheral configurations for ESP32-H2-DevKit boards
 * @{
 *
 * The peripheral configurations in this file are valid for Espressif
 * ESP32-H2-DevKitx boards that use the ESP32-H2-MINI-1x module.
 *
 * Since the number of GPIOs and their possible uses on the ESP32-H2 are quite
 * limited, these peripheral configurations can also be used for most other
 * ESP32-H2 boards. Any modifications required for specific applications
 * can be overridden by \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * For detailed information about the peripheral configuration for ESP32-H2
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
 * For generic boards, all ADC pins that are broken out are declared as ADC
 * channels.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#  define ADC_GPIOS     { GPIO1, GPIO2, GPIO3, GPIO4, GPIO5 }
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
#  define I2C0_SPEED    I2C_SPEED_FAST    /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#  define I2C0_SCL      GPIO10            /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#  define I2C0_SDA      GPIO11            /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * For generic boards, two PWM devices are configured. Generally, all output
 * pins could be used as PWM channels.
 * @{
 */

/**
 * @brief Declaration of the channels for device PWM_DEV(0), at maximum PWM_CHANNEL_NUM_DEV_MAX.
 *
 * @note PWM_DEV(0) contains only GPIOs that are not used for I2C, SPI
 *       or UART on this board. As long as the according PWM device is not
 *       initialized with the `pwm_init` function, the GPIOs declared for
 *       this device can be used for the other purposes.
 */
#ifndef PWM0_GPIOS
#  define PWM0_GPIOS    { GPIO2, GPIO12, GPIO22 }
#endif

/**
 * @brief Declaration of the channels for device PWM_DEV(1), at maximum PWM_CHANNEL_NUM_DEV_MAX.
 *
 * @note PMW_DEV(1) contains pins that are also used by SPI_DEV(0) or as ADC
 *       channel. However, as long as the according PWM device is not
 *       initialized with the `pwm_init` function, the GPIOs declared for
 *       this device can be used for the other purposes.
 */
#ifndef PWM1_GPIOS
#  define PWM1_GPIOS    { GPIO1, GPIO3, GPIO5 }
#endif

/** @} */

/**
 * @name    SPI configuration
 *
 * @note The GPIOs listed in the configuration are only initialized as SPI
 * signals when the corresponding SPI interface is used for the first time
 * by either calling the `spi_init_cs` function or the `spi_acquire`
 * function. That is, they are not allocated as SPI signals before and can
 * be used for other purposes as long as the SPI interface is not used.
 * @{
 */
#ifndef SPI0_CTRL
#  define SPI0_CTRL     FSPI    /**< FSPI is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#  define SPI0_SCK      GPIO4   /**< FSPI SCK (pin FSPICLK) */
#endif
#ifndef SPI0_MISO
#  define SPI0_MISO     GPIO5   /**< FSPI MISO (pin FSPIQ) */
#endif
#ifndef SPI0_MOSI
#  define SPI0_MOSI     GPIO0   /**< FSPI MOSI (pin FSPID) */
#endif
#ifndef SPI0_CS0
#  define SPI0_CS0      GPIO1  /**< FSPI CS0 (pin FSPICS0) */
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * ESP32-H2 provides 2 UART interfaces:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is not used.<br>
 *
 * @{
 */
#define UART0_TXD   GPIO24  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO23  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
