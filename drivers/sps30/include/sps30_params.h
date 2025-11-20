/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_sps30
 * @brief       Default configuration for Sensirion SPS30 sensors devices
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @file
 * @{
 */

#include "board.h"
#include "sps30.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPS30 default configuration parameters
 * @{
 */
#ifndef SPS30_PARAM_I2C_DEV
#define SPS30_PARAM_I2C_DEV  (I2C_DEV(0))
#endif
#ifndef SPS30_PARAMS
#define SPS30_PARAMS         { .i2c_dev  = SPS30_PARAM_I2C_DEV }
#endif
#ifndef SPS30_SAUL_INFO
#define SPS30_SAUL_INFO      { .name = "sps30" }
#endif
/**@}*/

/**
 * @brief   SPS30 configuration
 */
static const sps30_params_t sps30_params[] =
{
    SPS30_PARAMS
};

/**
 * @brief   Define the number of configured sensors
 */
#define SPS30_NUM               ARRAY_SIZE(sps30_params)

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t sps30_saul_info[] =
{
    SPS30_SAUL_INFO
};

/**
 * @brief   Number of saul info structs
 */
#define SPS30_INFO_NUM ARRAY_SIZE(sps30_saul_info)

#ifdef __cplusplus
}
#endif

/** @} */
