/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_si1133
 *
 * @{
 * @file
 * @brief       Default configuration for SI1133
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#ifndef SI1133_PARAMS_H
#define SI1133_PARAMS_H

#include "board.h"
#include "si1133.h"
#include "saul_reg.h"
#include "si1133_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the Si1133
 * @{
 */
#ifndef SI1133_PARAM_I2C_DEV
#define SI1133_PARAM_I2C_DEV         I2C_DEV(0)
#endif

#ifndef SI1133_PARAM_ADDR
#define SI1133_PARAM_ADDR            SI1133_I2C_ADDRESS
#endif

#ifndef SI1133_PARAMS
#define SI1133_PARAMS                { .i2c_dev = SI1133_PARAM_I2C_DEV, \
                                       .address = SI1133_PARAM_ADDR }
#endif

#ifndef SI1133_SAUL_INFO
#define SI1133_SAUL_INFO             { .name = "si1133" }
#endif
/**@}*/

/**
 * @brief   Configure Si1133
 */
static const si1133_params_t si1133_params[] =
{
    SI1133_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
saul_reg_t si1133_saul_reg_info[] =
{
    SI1133_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* SI1133_PARAMS_H */
/** @} */
