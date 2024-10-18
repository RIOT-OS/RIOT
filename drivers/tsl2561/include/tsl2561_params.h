/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tsl2561
 *
 * @{
 * @file
 * @brief       Default configuration for TSL2561
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef TSL2561_PARAMS_H
#define TSL2561_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "tsl2561.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the TSL2561
 * @{
 */
#ifndef TSL2561_PARAM_I2C_DEV
#define TSL2561_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef TSL2561_PARAM_ADDR
#define TSL2561_PARAM_ADDR            TSL2561_ADDR_FLOAT
#endif
#ifndef TSL2561_PARAM_GAIN
#define TSL2561_PARAM_GAIN            TSL2561_GAIN_1X
#endif
#ifndef TSL2561_PARAM_INTEGRATION
#define TSL2561_PARAM_INTEGRATION     TSL2561_INTEGRATIONTIME_402MS
#endif

#ifndef TSL2561_PARAMS
#define TSL2561_PARAMS                { .i2c_dev     = TSL2561_PARAM_I2C_DEV, \
                                        .addr        = TSL2561_PARAM_ADDR,    \
                                        .gain        = TSL2561_PARAM_GAIN,    \
                                        .integration = TSL2561_PARAM_INTEGRATION }
#endif
#ifndef TSL2561_SAUL_INFO
#define TSL2561_SAUL_INFO             { .name= "tsl2561" }
#endif
/**@}*/

/**
 * @brief   Configure TSL2561
 */
static const tsl2561_params_t tsl2561_params[] =
{
    TSL2561_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
saul_reg_info_t tsl2561_saul_info[] =
{
    TSL2561_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TSL2561_PARAMS_H */
