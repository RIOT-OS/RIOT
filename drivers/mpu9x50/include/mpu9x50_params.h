/*
 * Copyright (C) 2017   Inria
 *               2019   HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9x50
 * @{
 *
 * @file
 * @brief       Default configuration for MPU9X50 (MPU9150 and MPU9250) devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */

#ifndef MPU9X50_PARAMS_H
#define MPU9X50_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "mpu9x50.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Default configuration parameters for the MPU9X50 driver
 * @{
 */
#ifndef MPU9X50_PARAM_I2C
#define MPU9X50_PARAM_I2C         I2C_DEV(0)
#endif
#ifndef MPU9X50_PARAM_ADDR
#define MPU9X50_PARAM_ADDR        (MPU9X50_HW_ADDR_HEX_68)
#endif
#ifndef MPU9X50_PARAM_COMP_ADDR
#define MPU9X50_PARAM_COMP_ADDR   (MPU9X50_COMP_ADDR_HEX_0C)
#endif
#ifndef MPU9X50_PARAM_SAMPLE_RATE
#define MPU9X50_PARAM_SAMPLE_RATE (MPU9X50_DEFAULT_SAMPLE_RATE)
#endif

#ifndef MPU9X50_PARAMS
#define MPU9X50_PARAMS            { .i2c         = MPU9X50_PARAM_I2C,       \
                                    .addr        = MPU9X50_PARAM_ADDR,      \
                                    .comp_addr   = MPU9X50_PARAM_COMP_ADDR, \
                                    .sample_rate = MPU9X50_PARAM_SAMPLE_RATE }
#endif
#ifndef MPU9X50_SAUL_INFO
#define MPU9X50_SAUL_INFO         { .name = "mpu9x50" }
#endif
/**@}*/

/**
 * @brief   MPU9X50 configuration
 */
static const mpu9x50_params_t mpu9x50_params[] =
{
    MPU9X50_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mpu9x50_saul_info[] =
{
    MPU9X50_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* MPU9X50_PARAMS_H */
/** @} */
