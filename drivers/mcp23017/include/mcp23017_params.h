/*
 * Copyright (C) 2017 HAW HAmburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp23017
 * @{
 *
 * @file
 * @brief       Default configuration for MCP23017 devices
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 */

#ifndef MCP23017_PARAMS_H
#define MCP23017_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "mcp23017.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the MCP23017 driver
 * @{
 */
#ifndef MCP23017_PARAM_I2C
#define MCP23017_PARAM_I2C          (I2C_DEV(0))
#endif
#ifndef MCP23017_PARAM_ADDR
#define MCP23017_PARAM_ADDR         (MCP23017_ADDR_20)
#endif
#ifndef MCP23017_PARAMS
#define MCP23017_PARAMS             { .int_a_pin = 11, \
                                      .int_b_pin = 12 }
#endif
/**@}*/

/**
 * @brief   Allocation of MCP23017 configuration
 */
static const mcp23017_params_t mcp23017_params[] = {
    MCP23017_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* MCP23017_PARAMS_H */
/** @} */
