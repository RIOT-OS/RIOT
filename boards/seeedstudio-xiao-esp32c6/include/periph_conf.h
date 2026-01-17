/*
 * SPDX-FileCopyrightText: 2026 Stepan Konoplev
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-xiao-esp32c6
 * @{
 *
 * @file
 * @brief       Peripheral configurations for the Seeed Studio XIAO ESP32-C6
 *
 * @author      Stepan Konoplev <stepan.konoplev@haw-hamburg.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   I2C configuration
 * @{
 */
#ifndef I2C0_SDA
#  define I2C0_SDA    GPIO22             /**< I2C SDA pin */
#endif

#ifndef I2C0_SCL
#  define I2C0_SCL    GPIO23             /**< I2C SCL pin */
#endif

#ifndef I2C0_SPEED
#  define I2C0_SPEED  I2C_SPEED_FAST     /**< I2C bus speed */
#endif
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
#ifndef SPI0_CTRL
#  define SPI0_CTRL   FSPI               /**< SPI controller used */
#endif

#ifndef SPI0_SCK
#  define SPI0_SCK    GPIO19             /**< SPI SCK pin */
#endif

#ifndef SPI0_MOSI
#  define SPI0_MOSI   GPIO18             /**< SPI MOSI pin */
#endif

#ifndef SPI0_MISO
#  define SPI0_MISO   GPIO20             /**< SPI MISO pin */
#endif

#ifndef SPI0_CS0
#  define SPI0_CS0    GPIO21             /**< SPI CS0 pin */
#endif
/** @} */



/**
 * @name    ADC channel configuration
 * @{
 */
/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the `adc_init` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#  define ADC_GPIOS   { GPIO0, GPIO1, GPIO2 }
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * PWM_DEV(0) shares its GPIOs with the ADC channels on this board.
 * Generally, all output pins could be used as PWM channels.
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
#  define PWM0_GPIOS  { GPIO0, GPIO1, GPIO2 }
#endif
/** @} */

/**
 * @name   UART configuration
 *
 * Seeed Studio XIAO ESP32-C6 provides only 1 UART interface.
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 *
 * @{
 */
#define UART0_TXD   GPIO16              /**< UART TxD pin */
#define UART0_RXD   GPIO17              /**< UART RxD pin */
/** @} */


/* Include common peripheral definitions */
#include "periph_conf_common.h"

#ifdef __cplusplus
}
#endif

/** @} */
