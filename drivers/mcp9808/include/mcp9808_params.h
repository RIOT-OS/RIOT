/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp9808
 *
 * @{
 * @file
 * @brief       Default configuration for MCP9808
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef MCP9808_PARAMS_H
#define MCP9808_PARAMS_H

#include "board.h"
#include "mcp9808.h"
#include "mcp9808_constants.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the MCP9808
 * @{
 */
#ifndef MCP9808_PARAM_I2C_DEV
#define MCP9808_PARAM_I2C_DEV       I2C_DEV(0)
#endif
#ifndef MCP9808_PARAM_I2C_ADDR
#define MCP9808_PARAM_I2C_ADDR      MCP9808_ADDR
#endif
#ifndef MCP9808_PARAM_RESOLUTION
#define MCP9808_PARAM_RESOLUTION    MCP9808_RES_0_0625
#endif

#ifndef MCP9808_PARAMS
#define MCP9808_PARAMS              { .i2c_dev    = MCP9808_PARAM_I2C_DEV,  \
                                      .i2c_addr   = MCP9808_PARAM_I2C_ADDR, \
                                      .resolution = MCP9808_PARAM_RESOLUTION }
#endif
#ifndef MCP9808_SAUL_INFO
#define MCP9808_SAUL_INFO           { .name = "mcp9808" }
#endif
/**@}*/

/**
 * @brief   Configure MCP9808
 */
static const mcp9808_params_t mcp9808_params[] =
{
    MCP9808_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t mcp9808_saul_info[] =
{
    MCP9808_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* MCP9808_PARAMS_H */
/** @} */
