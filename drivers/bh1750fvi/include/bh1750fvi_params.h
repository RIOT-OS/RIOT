/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_bh1750fvi
 *
 * @{
 * @file
 * @brief       Default configuration for BH1750FVI devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "board.h"
#include "bh1750fvi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for BH1750FVI devices
 * @{
 */
#ifndef BH1750FVI_PARAM_I2C
#define BH1750FVI_PARAM_I2C         I2C_DEV(0)
#endif
#ifndef BH1750FVI_PARAM_ADDR
#define BH1750FVI_PARAM_ADDR        (BH1750FVI_DEFAULT_ADDR)
#endif

#ifndef BH1750FVI_PARAMS
#define BH1750FVI_PARAMS            { .i2c = BH1750FVI_PARAM_I2C,   \
                                      .addr = BH1750FVI_PARAM_ADDR }
#endif
/**@}*/

/**
 * @brief   BH1750FVI configuration
 */
static const bh1750fvi_params_t bh1750fvi_params[] =
{
    BH1750FVI_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
