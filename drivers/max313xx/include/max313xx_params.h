/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_max313xx_config  MAX313xx compile-time configuration
 * @ingroup     config_drivers_sensors
 * @brief       Default compile-time configuration for the MAX313xx driver.
 * @{
 *
 * @file
 * @brief       Default configuration parameters for the MAX313xx driver
 *
 * @author      Jakob Müller <ja.mueller@tuhh.de>
 */

#include "board.h"
#include "periph/i2c.h"
#include "max313xx.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX313XX_PARAM_I2C
#  define MAX313XX_PARAM_I2C        I2C_DEV(0)	/**< Default I2C bus */
#endif

/** @brief Default parameter set for MAX313xx devices */
#ifndef MAX313XX_PARAMS
#  define MAX313XX_PARAMS {    \
    .i2c = MAX313XX_PARAM_I2C, \
}
#endif

/**
 * @brief Default parameter set for MAX313XX devices
 */
static const max313xx_params_t max313xx_params[] = {
    MAX313XX_PARAMS
};

/** @} */

#ifdef __cplusplus
}
#endif
