/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ds75lx
 *
 * @{
 * @file
 * @brief       Default configuration for DS75LX
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef DS75LX_PARAMS_H
#define DS75LX_PARAMS_H

#include "board.h"
#include "ds75lx.h"
#include "ds75lx_internals.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the DS75LX
 * @{
 */
#ifndef DS75LX_PARAM_I2C_DEV
#define DS75LX_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef DS75LX_PARAM_I2C_ADDR
#define DS75LX_PARAM_I2C_ADDR        DS75LX_ADDR
#endif
#ifndef DS75LX_PARAM_RESOLUTION
#define DS75LX_PARAM_RESOLUTION      DS75LX_RESOLUTION_10
#endif

#ifndef DS75LX_PARAMS
#define DS75LX_PARAMS                { .i2c        = DS75LX_PARAM_I2C_DEV,  \
                                       .addr       = DS75LX_PARAM_I2C_ADDR, \
                                       .resolution = DS75LX_PARAM_RESOLUTION }
#endif
#ifndef DS75LX_SAUL_INFO
#define DS75LX_SAUL_INFO             { .name = "ds75lx" }
#endif
/**@}*/

/**
 * @brief   Configure DS75LX
 */
static const ds75lx_params_t ds75lx_params[] =
{
    DS75LX_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t ds75lx_saul_info[] =
{
    DS75LX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DS75LX_PARAMS_H */
