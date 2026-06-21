/*
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIS331DLH_PARAM_I2C
#define LIS331DLH_PARAM_I2C      I2C_DEV(0)
#endif

#ifndef LIS331DLH_PARAM_ADDR
#define LIS331DLH_PARAM_ADDR     LIS331DLH_ADDR_DEFAULT
#endif

#ifndef LIS331DLH_PARAM_SCALE
#define LIS331DLH_PARAM_SCALE    LIS331DLH_SCALE_2G
#endif

#ifndef LIS331DLH_PARAM_RATE
#define LIS331DLH_PARAM_RATE     LIS331DLH_RATE_50HZ
#endif

#ifndef LIS331DLH_PARAMS
#define LIS331DLH_PARAMS         { .i2c = LIS331DLH_PARAM_I2C, \
                                   .addr = LIS331DLH_PARAM_ADDR, \
                                   .scale = LIS331DLH_PARAM_SCALE, \
                                   .rate = LIS331DLH_PARAM_RATE }
#endif

static const lis331dlh_params_t lis331dlh_params[] =
{
    LIS331DLH_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
