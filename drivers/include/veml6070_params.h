/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_veml6070
 *
 * @{
 * @file
 * @brief       Default configuration for VEML6070
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef VEML6070_PARAMS_H
#define VEML6070_PARAMS_H

#include "board.h"
#include "veml6070.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the VEML6070
 * @{
 */
#ifndef VEML6070_PARAM_I2C_DEV
#define VEML6070_PARAM_I2C_DEV          I2C_DEV(0)
#endif
#ifndef VEML6070_PARAM_ITIME
#define VEML6070_PARAM_ITIME            VEML6070_1_T
#endif

#define VEML6070_PARAMS_DEFAULT        { .i2c_dev = VEML6070_PARAM_I2C_DEV,  \
                                         .itime   = VEML6070_PARAM_ITIME }
/**@}*/

/**
 * @brief   Configure VEML6070
 */
static const veml6070_params_t veml6070_params[] =
{
#ifdef VEML6070_PARAMS_BOARD
    VEML6070_PARAMS_BOARD,
#else
    VEML6070_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t veml6070_saul_reg_info[] =
{
    {
        .name = "veml6070-uv"
    }
};

#ifdef __cplusplus
}
#endif

#endif /* VEML6070_PARAMS_H */
/** @} */
