/*
 * Copyright (C) 2023 LIG
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lis2de12
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Léo <leo.cordier@etu.esisar.grenoble-inp.fr>
 */

#ifndef LIS2DE12_PARAMS_H
#define LIS2DE12_PARAMS_H

#include "board.h"
#include "lis2de12.h"
#include "lis2de12_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef LIS2DE12_PARAM_I2C
#define LIS2DE12_PARAM_I2C  LIS2DE12_I2C_DEV
#endif

#ifndef LIS2DE12_PARAM_ADDR
#define LIS2DE12_PARAM_ADDR  LIS2DE12_I2C_ADDRESS
#endif

#ifndef LIS2DE12_PARAMS
#define LIS2DE12_PARAMS     { \
                                .i2c = LIS2DE12_PARAM_I2C, \
                                .addr = LIS2DE12_PARAM_ADDR \
                            }
#endif

#ifndef LIS2DE12_PARAM_SCALE
#define LIS2DE12_PARAM_SCALE        LIS2DE12_SCALE_2G
#endif
#ifndef LIS2DE12_PARAM_RATE
#define LIS2DE12_PARAM_RATE         LIS2DE12_RATE_100HZ
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const lis2de12_params_t lis2de12_params[] =
{
    LIS2DE12_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* LIS2DE12_PARAMS_H */
/** @} */
