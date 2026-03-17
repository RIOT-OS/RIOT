/*
 * SPDX-FileCopyrightText: 2026 Jakob Müller <ja.mueller@tuhh.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    drivers_max31343_config  MAX31343 compile-time configuration
 * @ingroup     config_drivers_sensors
 * @brief       Default compile-time configuration for the MAX31343 driver.
 * @{
 *
 * @file
 * @brief       Default configuration parameters for the MAX31343 driver
 *
 * @author      Jakob Müller <ja.mueller@tuhh.de>
 */

#pragma once

#include "board.h"
#include "periph/i2c.h"
#include "max31343.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX31343_PARAM_I2C
#define MAX31343_PARAM_I2C        I2C_DEV(0)	/**< Default I2C bus */
#endif

#ifndef MAX31343_PARAM_SQW_FREQ
#define MAX31343_PARAM_SQW_FREQ     (MAX31343_SQW_1HZ)	/**< Default SQW frequency */
#endif

#ifndef MAX31343_PARAM_TRICKLE_ENABLE
#define MAX31343_PARAM_TRICKLE_ENABLE  false    /**< Default trickle charger disabled */
#endif

#ifndef MAX31343_PARAM_TRICKLE_DIODE
#define MAX31343_PARAM_TRICKLE_DIODE  MAX31343_TRICKLE_DIODE_SCHOTTKY   /**< Default trickle charger diode */
#endif

#ifndef MAX31343_PARAM_TRICKLE_RES
#define MAX31343_PARAM_TRICKLE_RES  MAX31343_TRICKLE_RES_11K    /**< Default trickle charger resistor */
#endif

#ifndef MAX31343_PARAMS
#define MAX31343_PARAMS {                                   \
    .i2c = MAX31343_PARAM_I2C,                              \
    .sqw_freq = MAX31343_PARAM_SQW_FREQ,                    \
    .trickle_enable = MAX31343_PARAM_TRICKLE_ENABLE,        \
    .trickle_diode = MAX31343_PARAM_TRICKLE_DIODE,          \
    .trickle_res = MAX31343_PARAM_TRICKLE_RES,              \
}                                                           /**< Default parameter set */
#endif

/**
 * @brief Default parameter set for MAX31343 devices
 */
static const max31343_params_t max31343_params[] = {
    MAX31343_PARAMS
};

/** @} */

#ifdef __cplusplus
}
#endif
