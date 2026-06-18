/*
 * SPDX-FileCopyrightText: 2026 Arseny Sukhman
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once


/**
 * @ingroup     drivers_lis331dlh
 * @{
 *
 * @file
 * @brief       Default configuration for LIS331DLH devices
 */


#include "board.h"
#include "lis331dlh.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef LIS331DLH_PARAM_I2C
/**
 * @brief   I2C device used by the LIS331DLH
 */
#define LIS331DLH_PARAM_I2C         (I2C_DEV(0))
#endif
#ifndef LIS331DLH_PARAM_ADDR
/**
 * @brief   I2C address of the LIS331DLH
 */
#define LIS331DLH_PARAM_ADDR        (LIS331DLH_I2C_ADDR_0)
#endif
#ifndef LIS331DLH_PARAM_SCALE
/**
 * @brief   Default full-scale range
 */
#define LIS331DLH_PARAM_SCALE       (LIS331DLH_SCALE_2G)
#endif
#ifndef LIS331DLH_PARAM_ODR
/**
 * @brief   Default output data rate
 */
#define LIS331DLH_PARAM_ODR         (LIS331DLH_ODR_100HZ)
#endif

#ifndef LIS331DLH_PARAMS
/**
 * @brief   Default LIS331DLH parameter set
 */
#define LIS331DLH_PARAMS            { .i2c   = LIS331DLH_PARAM_I2C,   \
                                      .addr  = LIS331DLH_PARAM_ADDR,  \
                                      .scale = LIS331DLH_PARAM_SCALE, \
                                      .odr   = LIS331DLH_PARAM_ODR }
#endif
#ifndef LIS331DLH_SAUL_INFO
/**
 * @brief   Default SAUL information for the LIS331DLH
 */
#define LIS331DLH_SAUL_INFO         { .name = "lis331dlh" }
#endif
/** @} */

/**
 * @brief   LIS331DLH configuration
 */
static const lis331dlh_params_t lis331dlh_params[] =
{
    LIS331DLH_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lis331dlh_saul_info[] =
{
    LIS331DLH_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
