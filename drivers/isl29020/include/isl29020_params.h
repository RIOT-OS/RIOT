/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_isl29020
 *
 * @{
 * @file
 * @brief       Default configuration for ISL29020 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ISL29020_PARAMS_H
#define ISL29020_PARAMS_H

#include "board.h"
#include "isl29020.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters
 * @{
 */
#ifndef ISL29020_PARAM_I2C
#define ISL29020_PARAM_I2C              I2C_DEV(0)
#endif
#ifndef ISL29020_PARAM_ADDR
#define ISL29020_PARAM_ADDR             (CONFIG_ISL29020_DEFAULT_ADDRESS)
#endif
#ifndef ISL29020_PARAM_RANGE
#define ISL29020_PARAM_RANGE            (ISL29020_RANGE_16K)
#endif
#ifndef ISL29020_PARAM_MODE
#define ISL29020_PARAM_MODE             (ISL29020_MODE_AMBIENT)
#endif

#ifndef ISL29020_PARAMS
#define ISL29020_PARAMS                 { .i2c   = ISL29020_PARAM_I2C,   \
                                          .addr  = ISL29020_PARAM_ADDR,  \
                                          .range = ISL29020_PARAM_RANGE, \
                                          .mode  = ISL29020_PARAM_MODE }
#endif
#ifndef ISL29020_SAUL_INFO
#define ISL29020_SAUL_INFO              { .name = "isl29020" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const isl29020_params_t isl29020_params[] =
{
    ISL29020_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t isl29020_saul_info[] =
{
    ISL29020_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ISL29020_PARAMS_H */
