/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

#include "board.h"
#include "periph/i2c.h"
#include "max31343.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX31343_PARAM_I2C
#  define MAX31343_PARAM_I2C        I2C_DEV(0)	/**< Default I2C bus */
#endif

#ifndef MAX31343_PARAMS
#  define MAX31343_PARAMS {                                 \
    .i2c = MAX31343_PARAM_I2C,                              \
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
