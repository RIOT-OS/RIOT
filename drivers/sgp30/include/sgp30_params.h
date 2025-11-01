/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sgp30
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "board.h"
#include "sgp30.h"
#include "sgp30_constants.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef SGP30_PARAM_I2C_DEV
#define SGP30_PARAM_I2C_DEV  (I2C_DEV(0))
#endif
#ifndef SGP30_PARAMS
#define SGP30_PARAMS         { .i2c_dev = SGP30_PARAM_I2C_DEV }
#endif
#ifndef SGP30_SAUL_INFO
#define SGP30_SAUL_INFO      { .name = "sgp30" }
#endif
/**@}*/

/**
 * @brief   SGP30 configuration
 */
static const sgp30_params_t sgp30_params[] =
{
    SGP30_PARAMS
};

/**
 * @brief   Define the number of configured sensors
 */
#define SGP30_NUM               ARRAY_SIZE(sgp30_params)

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t sgp30_saul_info[] =
{
    SGP30_SAUL_INFO
};

/**
 * @brief   Number of saul info structs
 */
#define SGP30_INFO_NUM ARRAY_SIZE(sgp30_saul_info)

#ifdef __cplusplus
}
#endif

/** @} */
