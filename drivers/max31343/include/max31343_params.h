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
#define MAX31343_PARAM_I2C        I2C_DEV(0)
#endif

#ifndef MAX31343_PARAM_USE_SQW
#define MAX31343_PARAM_USE_SQW      (0)
#endif

#ifndef MAX31343_PARAM_SQW_FREQ
#define MAX31343_PARAM_SQW_FREQ     (MAX31343_SQW_1HZ)
#endif

#ifndef MAX31343_PARAM_USE_TEMP_AUTOMODE
#define MAX31343_PARAM_USE_TEMP_AUTOMODE   (0)
#endif

#ifndef MAX31343_PARAM_TEMP_AUTOMODE_ENABLE
#define MAX31343_PARAM_TEMP_AUTOMODE_ENABLE (1)
#endif

#ifndef MAX31343_PARAM_TEMP_TTSINT
#define MAX31343_PARAM_TEMP_TTSINT  (MAX31343_TTSINT_16S)
#endif

#ifndef MAX31343_PARAMS
#define MAX31343_PARAMS {                                   \
    .i2c = MAX31343_PARAM_I2C,                              \
    .use_sqw = MAX31343_PARAM_USE_SQW,                      \
    .sqw_freq = MAX31343_PARAM_SQW_FREQ,                    \
    .use_temp_automode = MAX31343_PARAM_USE_TEMP_AUTOMODE,  \
    .temp_automode_enable = MAX31343_PARAM_TEMP_AUTOMODE_ENABLE, \
    .temp_ttsint = MAX31343_PARAM_TEMP_TTSINT,              \
}
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
