/*
 * SPDX-FileCopyrightText: 2026 Mathis LECRIVAIN <lecrivain.mathis@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_max1161x
 * @{
 *
 * @file
 * @brief       Default configuration for MAX1161X devices
 *
 * @author      Mathis Lécrivain <lecrivain.mathis@gmail.com>
 */

#include "board.h"
#include "max1161x.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_max1161x_config   MAX1161X driver compile configurations
 * @ingroup drivers_max1161x
 * @ingroup config_drivers_sensors
 * @{
 */

/** @brief  I2C device to use */
#ifndef MAX1161X_PARAM_I2C
#  define MAX1161X_PARAM_I2C (I2C_DEV(0))
#endif

/** @brief  I2C address */
#ifndef MAX1161X_PARAM_ADDR
#  define MAX1161X_PARAM_ADDR (MAX1161X_I2C_ADDRESS)
#endif

/** @brief  Reference mode */
#ifndef MAX1161X_PARAM_REFERENCE
#  define MAX1161X_PARAM_REFERENCE (MAX1161X_REF_DEFAULT)
#endif

/** @brief  Clock source */
#ifndef MAX1161X_PARAM_CLOCK
#  define MAX1161X_PARAM_CLOCK (MAX1161X_CLOCK_DEFAULT)
#endif

/** @brief  Unipolar/bipolar mode */
#ifndef MAX1161X_PARAM_UNIBIPOLAR
#  define MAX1161X_PARAM_UNIBIPOLAR (MAX1161X_UNIBIPOLAR_DEFAULT)
#endif

/** @brief  Scan mode */
#ifndef MAX1161X_PARAM_SCAN
#  define MAX1161X_PARAM_SCAN (MAX1161X_SCAN_DEFAULT)
#endif

/** @brief  Channel */
#ifndef MAX1161X_PARAM_CHANNEL
#  define MAX1161X_PARAM_CHANNEL (MAX1161X_CHANNEL_DEFAULT)
#endif

/** @brief  Single-ended/differential mode */
#ifndef MAX1161X_PARAM_SGLDIFF
#  define MAX1161X_PARAM_SGLDIFF (MAX1161X_SGLDIFF_DEFAULT)
#endif
/** @} */

/**
 * @brief   MAX1161X driver configuration structures
 */
#ifndef MAX1161X_PARAMS
#  define MAX1161X_PARAMS { .i2c = MAX1161X_PARAM_I2C,               \
                            .addr = MAX1161X_PARAM_ADDR,             \
                            .ref = MAX1161X_PARAM_REFERENCE,         \
                            .clock = MAX1161X_PARAM_CLOCK,           \
                            .unibipolar = MAX1161X_PARAM_UNIBIPOLAR, \
                            .scan = MAX1161X_PARAM_SCAN,             \
                            .channel = MAX1161X_PARAM_CHANNEL,       \
                            .sgldiff = MAX1161X_PARAM_SGLDIFF }
#endif

/**
 * @brief   MAX1161X driver SAUL registry information structures
 */
#ifndef MAX1161X_SAUL_INFO
#  define MAX1161X_SAUL_INFO { .name = "max1161x" }
#endif

/**
 * @brief   MAX1161X configuration
 */
static const max1161x_params_t max1161x_params[] = {
    MAX1161X_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t max1161x_saul_info[] = {
    MAX1161X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
