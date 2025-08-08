/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_tcs37727
 *
 * @{
 * @file
 * @brief       Default configuration for TCS37727 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "board.h"
#include "tcs37727.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Set default configuration parameters for TCS37727 devices
 * @{
 */
#ifndef TCS37727_PARAM_I2C
#define TCS37727_PARAM_I2C          I2C_DEV(0)
#endif
#ifndef TCS37727_PARAM_ADDR
#define TCS37727_PARAM_ADDR         (TCS37727_I2C_ADDRESS)
#endif
#ifndef TCS37727_PARAM_ATIME
#define TCS37727_PARAM_ATIME        (CONFIG_TCS37727_ATIME_DEFAULT)
#endif

#ifndef TCS37727_PARAMS
#define TCS37727_PARAMS             { .i2c   = TCS37727_PARAM_I2C,  \
                                      .addr  = TCS37727_PARAM_ADDR, \
                                      .atime = TCS37727_PARAM_ATIME }
#endif
#ifndef TCS37727_SAUL_INFO
#define TCS37727_SAUL_INFO          { .name = "tcs37727" }
#endif
/**@}*/

/**
 * @brief   TCS37727 configuration
 */
static const tcs37727_params_t tcs37727_params[] =
{
    TCS37727_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t tcs37727_saul_info[] =
{
    TCS37727_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
