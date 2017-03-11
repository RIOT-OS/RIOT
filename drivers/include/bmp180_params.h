/*
 * Copyright (C) 2016 Inria
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bmp180
 *
 * @{
 * @file
 * @brief       Default configuration for BMP180
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BMP180_PARAMS_H
#define BMP180_PARAMS_H

#include "board.h"
#include "bmp180.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the BMP180
 * @{
 */
#ifndef BMP180_PARAM_I2C_DEV
#define BMP180_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef BMP180_PARAM_MODE
#define BMP180_PARAM_MODE            BMP180_ULTRALOWPOWER
#endif

#define BMP180_PARAMS_DEFAULT        { .i2c_dev = BMP180_PARAM_I2C_DEV,  \
                                       .mode    = BMP180_PARAM_MODE }
/**@}*/

/**
 * @brief   Configure BMP180
 */
static const bmp180_params_t bmp180_params[] =
{
#ifdef BMP180_PARAMS_BOARD
    BMP180_PARAMS_BOARD,
#else
    BMP180_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t bmp180_saul_reg_info[][2] =
{
    {
        { .name = "bmp180-temp" },
        { .name = "bmp180-press" }
    }
};

#ifdef __cplusplus
}
#endif

#endif /* BMP180_PARAMS_H */
/** @} */
