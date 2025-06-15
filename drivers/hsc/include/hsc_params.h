/*
 * SPDX-FileCopyrightText: 2020 Deutsches Zentrum für Luft- und Raumfahrt e.V. (DLR)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_hsc
 *
 * @{
 * @file
 * @brief       Default configuration for HSC
 *
 * @author      Quang Pham <phhr_quang@live.com>
 */

#include "board.h"
#include "hsc.h"
#include "hsc_internals.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the HSC
 * @{
 */
#ifndef HSC_PARAM_I2C_DEV
#define HSC_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef HSC_PARAM_I2C_ADDR
#define HSC_PARAM_I2C_ADDR        CONFIG_HSC_I2C_ADDR
#endif
#ifndef HSC_PARAM_RANGE
#define HSC_PARAM_RANGE           CONFIG_HSC_RANGE
#endif

#ifndef HSC_PARAMS
#define HSC_PARAMS                { .i2c_dev      = HSC_PARAM_I2C_DEV,  \
                                    .i2c_addr     = HSC_PARAM_I2C_ADDR, \
                                    .hsc_range    = HSC_PARAM_RANGE,    }
#endif
#ifndef HSC_SAUL_INFO
#define HSC_SAUL_INFO             { .name = "hsc" }
#endif
/**@}*/

/**
 * @brief   Configure HSC
 */
static const hsc_params_t hsc_params[] =
{
    HSC_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t hsc_saul_info[] =
{
    HSC_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
