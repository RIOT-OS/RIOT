/*
 * Copyright (C) 2016 Inria
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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


#include "board.h"
#include "bmp180.h"
#include "bmp180_internals.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the BMP180
 * @{
 */
#ifndef BMP180_PARAM_I2C_DEV
#define BMP180_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef BMP180_PARAM_I2C_ADDR
#define BMP180_PARAM_I2C_ADDR        BMP180_ADDR
#endif
#ifndef BMP180_PARAM_OVERSAMPLING
#define BMP180_PARAM_OVERSAMPLING    BMP180_ULTRALOWPOWER
#endif

#ifndef BMP180_PARAMS
#define BMP180_PARAMS                { .i2c_dev      = BMP180_PARAM_I2C_DEV,  \
                                       .i2c_addr     = BMP180_PARAM_I2C_ADDR, \
                                       .oversampling = BMP180_PARAM_OVERSAMPLING }
#endif
#ifndef BMP180_SAUL_INFO
#define BMP180_SAUL_INFO             { .name = "bmp180" }
#endif
/**@}*/

/**
 * @brief   Configure BMP180
 */
static const bmp180_params_t bmp180_params[] =
{
    BMP180_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t bmp180_saul_info[] =
{
    BMP180_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
