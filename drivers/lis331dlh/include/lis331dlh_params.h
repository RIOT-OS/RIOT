/*
 * SPDX-FileCopyrightText: 2026 Anastasiia
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_lis331dlh
 *
 * @{
 * @file
 * @brief       Default configuration for LIS331DLH devices
 */

#include "board.h"
#include "lis331dlh.h"
#include "saul_reg.h"

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
#define LIS331DLH_PARAMS         { .i2c   = LIS331DLH_PARAM_I2C,   \
                                   .addr  = LIS331DLH_PARAM_ADDR,  \
                                   .scale = LIS331DLH_PARAM_SCALE, \
                                   .rate  = LIS331DLH_PARAM_RATE }
#endif

#ifndef LIS331DLH_SAUL_INFO
#define LIS331DLH_SAUL_INFO      { .name = "lis331dlh" }
#endif

static const lis331dlh_params_t lis331dlh_params[] =
{
    LIS331DLH_PARAMS
};

static const saul_reg_info_t lis331dlh_saul_info[] =
{
    LIS331DLH_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
