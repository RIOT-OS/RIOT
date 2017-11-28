/*
 * Copyright (C) 2017   Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9150
 * @{
 *
 * @file
 * @brief       Default configuration for MPU9150 devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef MPU9150_PARAMS_H
#define MPU9150_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "mpu9150.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Default configuration parameters for the MPU9150 driver
 * @{
 */
#ifndef MPU9150_PARAM_I2C
#define MPU9150_PARAM_I2C         I2C_DEV(0)
#endif
#ifndef MPU9150_PARAM_ADDR
#define MPU9150_PARAM_ADDR        (MPU9150_HW_ADDR_HEX_68)
#endif
#ifndef MPU9150_PARAM_COMP_ADDR
#define MPU9150_PARAM_COMP_ADDR   (MPU9150_COMP_ADDR_HEX_0C)
#endif
#ifndef MPU9150_PARAM_SAMPLE_RATE
#define MPU9150_PARAM_SAMPLE_RATE (MPU9150_DEFAULT_SAMPLE_RATE)
#endif

#ifndef MPU9150_PARAMS
#define MPU9150_PARAMS            { .i2c         = MPU9150_PARAM_I2C,       \
                                    .addr        = MPU9150_PARAM_ADDR,      \
                                    .comp_addr   = MPU9150_PARAM_COMP_ADDR, \
                                    .sample_rate = MPU9150_PARAM_SAMPLE_RATE }
#endif
#ifndef MPU9150_SAUL_INFO
#define MPU9150_SAUL_INFO         { .name = "mpu9150" }
#endif
/**@}*/

/**
 * @brief   MPU9150 configuration
 */
static const mpu9150_params_t mpu9150_params[] =
{
    MPU9150_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mpu9150_saul_info[] =
{
    MPU9150_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* MPU9150_PARAMS_H */
/** @} */