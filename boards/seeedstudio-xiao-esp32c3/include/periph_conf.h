/*
 * SPDX-FileCopyrightText: 2025 David Picard
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-xiao-esp32c3
 * @brief       Peripheral configurations for the Seeed Studio ESP32-C3 Xiao board
 * @{
 *
 * For detailed information about the peripheral configuration for ESP32-C3
 * boards, see section \ref esp32_peripherals "Common Peripherals".
 *
 * To specify the board, add the following to the make command line:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * BOARD=seeedstudio-xiao-esp32c3 make ...
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_application_specific_configurations
 * "application-specific board configuration" if necessary.
 *
 * @file
 * @author      David Picard
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC channel configuration
 * @{
 */
/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * On this board, only three pins can be used as ADC channels.
 * Note that GPIO5 is linked to ADC2, which _"does not work properly"_,
 * and therefore can't be used as an ADC channel.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#define ADC_GPIOS   { GPIO2, GPIO3, GPIO4 }
#endif
/** @} */

/**
 * @name   I2C configuration
 *
 * The ESP32C3 has one I2C interface I2C_DEV(0).
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
#define I2C0_SCL    GPIO7           /**< SCL signal of I2C_DEV(0) */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO6           /**< SDA signal of I2C_DEV(0) */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * The ESP32C3 has one LED PWM device.
 * Pins that are not defined as I2C, SPI or UART are listed
 * as PWM channels.
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
#define PWM0_GPIOS  { GPIO2, GPIO3, GPIO4 }
#endif

/** @} */

/**
 * @name    SPI configuration
 *
 * @note
 * The GPIOs listed in the configuration are not initialized as SPI
 * signals until the corresponding SPI interface is used for the first time
 * by either calling the `spi_init_cs` function or the `spi_acquire`
 * function. That is, they are not allocated as SPI signals before and can
 * be used for other purposes as long as the SPI interface is not used.
 *
 * @{
 */
#ifndef SPI0_CTRL
#define SPI0_CTRL   FSPI    /**< FSPI is used as SPI_DEV(0) */
#endif
#ifndef SPI0_SCK
#define SPI0_SCK    GPIO8   /**< FSPI SCK signal routed to GPIO8 in GPIO matrix */
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO9   /**< FSPI SCK signal routed to GPIO9 in GPIO matrix */
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO10  /**< FSPI SCK signal routed to GPIO10 in GPIO matrix */
#endif
#ifndef SPI0_CS0
#define SPI0_CS0    GPIO5   /**< CS pin controlled in software */
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
