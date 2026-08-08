/*
 * SPDX-FileCopyrightText: 2026 RIOT Contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sht4x
 * @brief       Default configuration for Sensirion SHT4X devices
 * @file
 * @{
 */

#include "board.h"
#include "sht4x.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SHT4X_PARAM_I2C_DEV
#define SHT4X_PARAM_I2C_DEV     (I2C_DEV(0))
#endif

#ifndef SHT4X_PARAM_I2C_ADDR
#define SHT4X_PARAM_I2C_ADDR    (SHT4X_I2C_ADDR_1)
#endif

#ifndef SHT4X_PARAM_REPEAT
#define SHT4X_PARAM_REPEAT      (SHT4X_HIGH)
#endif

#ifndef SHT4X_PARAMS
#define SHT4X_PARAMS    { .i2c_dev  = SHT4X_PARAM_I2C_DEV,  \
                          .i2c_addr = SHT4X_PARAM_I2C_ADDR, \
                          .repeat   = SHT4X_PARAM_REPEAT    \
                        }
#endif

#ifndef SHT4X_SAUL_INFO
#define SHT4X_SAUL_INFO { .name = "sht4x1" }
#endif

#ifndef SHT4X_INFO_NUM
#define SHT4X_INFO_NUM ARRAY_SIZE(sht4x_saul_info)
#endif

static const sht4x_params_t sht4x_params[] =
{
    SHT4X_PARAMS
};

static const saul_reg_info_t sht4x_saul_info[] =
{
    SHT4X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
