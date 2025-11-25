/*
 * SPDX-FileCopyrightText: 2019 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_at24mac
 *
 * @{
 * @file
 * @brief       Default configuration for the AT24MAC chip
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "board.h"
#include "at24mac.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for AT24Mac driver
 * @{
 */
#ifndef AT24MAC_PARAM_I2C_DEV
#define AT24MAC_PARAM_I2C_DEV       I2C_DEV(0)
#endif
#ifndef AT24MAC_PARAM_I2C_ADDR
#define AT24MAC_PARAM_I2C_ADDR      (0x58)
#endif
#ifndef AT24MAC_PARAM_TYPE
#define AT24MAC_PARAM_TYPE          (AT24MAC6XX)
#endif

#ifndef AT24MAC_PARAMS
#define AT24MAC_PARAMS              { .i2c_dev      = AT24MAC_PARAM_I2C_DEV, \
                                      .i2c_addr     = AT24MAC_PARAM_I2C_ADDR,\
                                      .type         = AT24MAC_PARAM_TYPE     }
#endif
/**@}*/

/**
 * @brief   Configuration for AT24MACs
 */
static const at24mac_params_t at24mac_params[] =
{
    AT24MAC_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
