/*
 * SPDX-FileCopyrightText: 2020 Viktor Gal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_seesaw_soil
 *
 * @{
 * @file
 * @brief       Default configuration for Seesaw Soil devices
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 */

#include "board.h"
#include "seesaw_soil.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the SEESAW_SOIL driver
 * @{
 */
#ifndef SEESAW_SOIL_PARAM_I2C
#define SEESAW_SOIL_PARAM_I2C            I2C_DEV(0)
#endif
#ifndef SEESAW_SOIL_PARAM_ADDR
#define SEESAW_SOIL_PARAM_ADDR           (CONFIG_SEESAW_SOIL_I2C_ADDRESS)
#endif

#ifndef SEESAW_SOIL_PARAMS
#define SEESAW_SOIL_PARAMS               { .i2c  = SEESAW_SOIL_PARAM_I2C,  \
                                           .addr = SEESAW_SOIL_PARAM_ADDR }
#endif
#ifndef SEESAW_SOIL_SAUL_INFO
#define SEESAW_SOIL_SAUL_INFO            { .name = "seesaw_soil" }
#endif
/**@}*/

/**
 * @brief   Seesaw Soil configuration
 */
static const seesaw_soil_params_t seesaw_soil_params[] =
{
    SEESAW_SOIL_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t seesaw_soil_saul_info[] =
{
    SEESAW_SOIL_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
