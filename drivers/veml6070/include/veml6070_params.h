/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_veml6070
 *
 * @{
 * @file
 * @brief       Default configuration for VEML6070
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "board.h"
#include "veml6070.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the VEML6070
 * @{
 */
#ifndef VEML6070_PARAM_I2C_DEV
#define VEML6070_PARAM_I2C_DEV          I2C_DEV(0)
#endif
#ifndef VEML6070_PARAM_ITIME
#define VEML6070_PARAM_ITIME            VEML6070_1_T
#endif

#ifndef VEML6070_PARAMS
#define VEML6070_PARAMS                { .i2c_dev = VEML6070_PARAM_I2C_DEV, \
                                         .itime   = VEML6070_PARAM_ITIME }
#endif
#ifndef VEML6070_SAUL_INFO
#define VEML6070_SAUL_INFO             { .name = "veml6070" }
#endif
/**@}*/

/**
 * @brief   Configure VEML6070
 */
static const veml6070_params_t veml6070_params[] =
{
    VEML6070_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t veml6070_saul_info[] =
{
    VEML6070_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
