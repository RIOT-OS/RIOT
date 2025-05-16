/*
 * Copyright (C) 2017 Freie Universität Berlin
 * Copyright (C) 2018 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_isl29125
 *
 * @{
 * @file
 * @brief       Default configuration for ISL29125 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */


#include "board.h"
#include "isl29125.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters
 * @{
 */
#ifndef ISL29125_PARAM_I2C
#define ISL29125_PARAM_I2C              I2C_DEV(0)
#endif
#ifndef ISL29125_PARAM_GPIO
#define ISL29125_PARAM_GPIO             (GPIO_PIN(0, 0))
#endif
#ifndef ISL29125_PARAM_RANGE
#define ISL29125_PARAM_RANGE            (ISL29125_RANGE_10K)
#endif
#ifndef ISL29125_PARAM_MODE
#define ISL29125_PARAM_MODE             (ISL29125_MODE_RGB)
#endif
#ifndef ISL29125_PARAM_RES
#define ISL29125_PARAM_RES              (ISL29125_RESOLUTION_16)
#endif

#ifndef ISL29125_PARAMS
#define ISL29125_PARAMS                 { .i2c   = ISL29125_PARAM_I2C,   \
                                          .gpio  = ISL29125_PARAM_GPIO,  \
                                          .range = ISL29125_PARAM_RANGE, \
                                          .mode  = ISL29125_PARAM_MODE,  \
                                          .res   = ISL29125_PARAM_RES }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const isl29125_params_t isl29125_params[] =
{
    ISL29125_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
