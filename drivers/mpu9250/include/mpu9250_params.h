/*
 * Copyright (C) 2017   Inria
 * Copyright 2018		Ville Hiltunen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9250
 * @{
 *
 * @file
 * @brief       Default configuration for MPU9250 devices
 *
 * @note        Originally for MPU9150. Variables renamed for MPU9250
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>,
 *              modified by Ville Hiltunen <hiltunenvillej@gmail.com>
 */

#ifndef MPU9250_PARAMS_H
#define MPU9250_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "mpu9250.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Default configuration parameters for the MPU9250 driver
 * @{
 */
#ifndef MPU9250_PARAM_I2C
#define MPU9250_PARAM_I2C         I2C_DEV(0)
#endif
#ifndef MPU9250_PARAM_ADDR
#define MPU9250_PARAM_ADDR        (MPU9250_HW_ADDR_HEX_68)
#endif
#ifndef MPU9250_PARAM_COMP_ADDR
#define MPU9250_PARAM_COMP_ADDR   (MPU9250_COMP_ADDR_HEX_0C)
#endif
#ifndef MPU9250_PARAM_SAMPLE_RATE
#define MPU9250_PARAM_SAMPLE_RATE (MPU9250_DEFAULT_SAMPLE_RATE)
#endif

#ifndef MPU9250_PARAMS
#define MPU9250_PARAMS            { .i2c         = MPU9250_PARAM_I2C,       \
                                    .addr        = MPU9250_PARAM_ADDR,      \
                                    .comp_addr   = MPU9250_PARAM_COMP_ADDR, \
                                    .sample_rate = MPU9250_PARAM_SAMPLE_RATE }
#endif
#ifndef MPU9250_SAUL_INFO
#define MPU9250_SAUL_INFO         { .name = "mpu9250" }
#endif
/**@}*/

/**
 * @brief   MPU9250 configuration
 */
static const mpu9250_params_t mpu9250_params[] =
{
    MPU9250_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mpu9250_saul_info[] =
{
    MPU9250_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* MPU9250_PARAMS_H */
/** @} */
