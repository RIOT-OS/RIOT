/*
 * SPDX-FileCopyrightText: 2026 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_ulanzi-tc001
 * @brief       Peripheral MCU configuration for Ulanzi TC001
 * @{
 *
 * @file
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
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
 */
#ifndef ADC_GPIOS
#define ADC_GPIOS   { GPIO34,   /* Battery Voltage */       \
                      GPIO35,   /* GL5516 Light Sensor */   \
                    }
#endif

/**
 * @name   I2C configuration
 *
 * SHT31 and DS1307 are connected to this bus.
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
 * @{
 */

/**
 * @brief We connect the buzzer to the first PWM channel
 */
#ifndef PWM0_GPIOS
#define PWM0_GPIOS  { GPIO15 }
#endif
/** @} */

/**
 * @name   UART configuration
 * @{
 */
#define UART0_TXD   GPIO1  /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO3  /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common peripheral definitions as last step */
#include "periph_conf_common.h"

/** @} */
