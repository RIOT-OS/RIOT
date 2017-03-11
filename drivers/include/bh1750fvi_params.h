/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bh1750fvi
 *
 * @{
 * @file
 * @brief       Default configuration for BH1750FVI devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BH1750FVI_PARAMS_H
#define BH1750FVI_PARAMS_H

#include "board.h"
#include "bh1750fvi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for BH1750FVI devices
 * @{
 */
#ifndef BH1750FVI_PARAM_I2C
#define BH1750FVI_PARAM_I2C         I2C_DEV(0)
#endif
#ifndef BH1750FVI_PARAM_ADDR
#define BH1750FVI_PARAM_ADDR        (BH1750FVI_DEFAULT_ADDR)
#endif
#ifndef BH1750FVI_PARAM_I2C_CLK
#define BH1750FVI_PARAM_I2C_CLK     (BH1750FVI_I2C_MAX_CLK)
#endif

#define BH1750FVI_PARAMS_DEFAULT    {.i2c = BH1750FVI_PARAM_I2C, \
                                     .addr = BH1750FVI_PARAM_ADDR, \
                                     .clk = BH1750FVI_PARAM_I2C_CLK}
/**@}*/

/**
 * @brief   BH1750FVI configuration
 */
static const bh1750fvi_params_t bh1750fvi_params[] =
{
#ifdef BH1750FVI_PARAMS_BOARD
    BH1750FVI_PARAMS_BOARD,
#else
    BH1750FVI_PARAMS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* BH1750FVI_PARAMS_H */
/** @} */
