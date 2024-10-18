/*
 * Copyright (C) 2020 Viktor Gal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_seesaw_soil
 *
 * @{
 * @file
 * @brief       Default configuration for Seesaw Soil devices
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 */

#ifndef SEESAW_SOIL_PARAMS_H
#define SEESAW_SOIL_PARAMS_H

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
#endif /* SEESAW_SOIL_PARAMS_H */
