/*
 * SPDX-FileCopyrightText: 2020 Nalys
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_bh1900nux
 *
 * @{
 * @file
 * @brief       Default configuration for BH1900NUX devices
 *
 * @author      Wouter Symons <wsymons@nalys-group.com>
 */

#include "board.h"
#include "bh1900nux.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for BH1900NUX devices
 * @{
 */
#ifndef BH1900NUX_PARAM_I2C
#define BH1900NUX_PARAM_I2C         I2C_DEV(0)
#endif
#ifndef BH1900NUX_PARAM_ADDR
#define BH1900NUX_PARAM_ADDR        (BH1900NUX_DEFAULT_ADDR)
#endif

#ifndef BH1900NUX_PARAMS
#define BH1900NUX_PARAMS            { .i2c = BH1900NUX_PARAM_I2C,   \
                                      .addr = BH1900NUX_PARAM_ADDR }
#endif
/**@}*/

/**
 * @brief   BH1900NUX configuration
 */
static const bh1900nux_params_t bh1900nux_params[] =
{
    BH1900NUX_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
