/*
 * Copyright (C) 2017   HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_mpl3115a2
 * @{
 *
 * @file
 * @brief       Default configuration for MPL3115A2 devices
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */


#include "board.h"
#include "saul_reg.h"
#include "mpl3115a2.h"
#include "mpl3115a2_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Default configuration parameters for the MPL3115A2 driver
 * @{
 */
#ifndef MPL3115A2_PARAM_I2C
#define MPL3115A2_PARAM_I2C         I2C_DEV(0)
#endif

#ifndef MPL3115A2_PARAM_ADDR
#define MPL3115A2_PARAM_ADDR        MPL3115A2_I2C_ADDRESS
#endif

#ifndef MPL3115A2_PARAM_RATIO
#define MPL3115A2_PARAM_RATIO       MPL3115A2_OS_RATIO_DEFAULT
#endif

#ifndef MPL3115A2_PARAMS
#define MPL3115A2_PARAMS            { .i2c   = MPL3115A2_PARAM_I2C,  \
                                      .addr  = MPL3115A2_PARAM_ADDR, \
                                      .ratio = MPL3115A2_PARAM_RATIO }
#endif
#ifndef MPL3115A2_SAUL_INFO
#define MPL3115A2_SAUL_INFO         { .name = "mpl3115a2" }
#endif
/**@}*/

/**
 * @brief   MPL3115A2 configuration
 */
static const mpl3115a2_params_t mpl3115a2_params[] =
{
    MPL3115A2_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mpl3115a2_saul_info[] =
{
    MPL3115A2_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
