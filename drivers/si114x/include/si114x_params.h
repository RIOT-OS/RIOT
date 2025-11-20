/*
 * Copyright (C) 2016-2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_si114x
 *
 * @{
 * @file
 * @brief       Default configuration for SI114x
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "board.h"
#include "si114x.h"
#include "saul_reg.h"
#include "si114x_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the Si114x
 * @{
 */
#ifndef SI114X_PARAM_I2C_DEV
#define SI114X_PARAM_I2C_DEV         I2C_DEV(0)
#endif

#ifndef SI114X_PARAMS
#define SI114X_PARAMS                { .i2c_dev     = SI114X_PARAM_I2C_DEV, \
                                       .led_current = SI114X_PS_LED1 }
#endif
#ifndef SI114X_SAUL_INFO
#define SI114X_SAUL_INFO             { .name = "si114x" }
#endif
/**@}*/

/**
 * @brief   Configure Si114x
 */
static const si114x_params_t si114x_params[] =
{
    SI114X_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
saul_reg_t si114x_saul_reg_info[] =
{
    SI114X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
