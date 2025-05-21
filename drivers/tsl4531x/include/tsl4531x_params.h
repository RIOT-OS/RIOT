/*
 * Copyright (C) 2017 Inria
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_tsl4531x
 *
 * @{
 * @file
 * @brief       Default configuration for tsl4531x light sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Daniel Petry <danielpetry@cantab.net>
 *
 * Derived from the default configuration for the tsl2561 driver.
 */

#include "board.h"
#include "saul_reg.h"
#include "tsl4531x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the TSL4531x
 * @{
 */
#ifndef TSL4531X_PARAM_I2C_DEV
#define TSL4531X_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef TSL4531X_PARAM_I2C_ADDR
#define TSL4531X_PARAM_I2C_ADDR        TSL45315_ADDR
#endif
#ifndef TSL4531X_PARAM_INTEGRATION
#define TSL4531X_PARAM_INTEGRATION     TSL4531X_INTEGRATE_400MS
#endif
#ifndef TSL4531X_LOW_POWER_MODE
#define TSL4531X_LOW_POWER_MODE        (false)
#endif
#ifndef TSL4531X_PARAM_PARTNO
#define TSL4531X_PARAM_PARTNO          TSL45315_PARTNO
#endif
#ifndef TSL4531X_PARAMS
#define TSL4531X_PARAMS                { .i2c_dev     = TSL4531X_PARAM_I2C_DEV, \
                                         .i2c_addr    = TSL4531X_PARAM_I2C_ADDR, \
                                         .integration_time = TSL4531X_PARAM_INTEGRATION, \
                                         .low_power_mode = TSL4531X_LOW_POWER_MODE, \
                                         .part_number = TSL4531X_PARAM_PARTNO }
#endif

#ifndef TSL4531X_SAUL_INFO
#define TSL4531X_SAUL_INFO             { .name = "tsl4531x" }
#endif
/**@}*/

/**
 * @brief   Configure TSL4531x
 */
static const tsl4531x_params_t tsl4531x_params[] =
{
    TSL4531X_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
saul_reg_info_t tsl4531x_saul_info[] =
{
    TSL4531X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
