/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_si70xx
 *
 * @{
 * @file
 * @brief       Default configuration for Si7006/13/20/21
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#include "board.h"
#include "si70xx.h"
#include "saul_reg.h"
#include "si70xx_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the Si7006/13/20/21 sensor
 * @{
 */
#ifndef SI70XX_PARAM_I2C_DEV
#define SI70XX_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef SI70XX_PARAM_ADDR
#define SI70XX_PARAM_ADDR            SI70XX_I2C_ADDRESS
#endif

#ifndef SI70XX_PARAMS
#define SI70XX_PARAMS                { .i2c_dev = SI70XX_PARAM_I2C_DEV, \
                                       .address = SI70XX_PARAM_ADDR }
#endif
#ifndef SI70XX_SAUL_INFO
#define SI70XX_SAUL_INFO             { .name = "si70xx" }
#endif
/**@}*/

/**
 * @brief   Configure Si7006/13/20/21
 */
static const si70xx_params_t si70xx_params[] =
{
    SI70XX_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t si70xx_saul_info[] =
{
    SI70XX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
