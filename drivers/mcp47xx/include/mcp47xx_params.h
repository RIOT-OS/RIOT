/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_mcp47xx
 * @brief       Default configuration for Microchip MCP47xx DAC with I2C interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */


#include "board.h"
#include "mcp47xx.h"
#include "saul_reg.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef MCP47XX_PARAM_VARIANT
/** Default MCP47xx variant */
#define MCP47XX_PARAM_VARIANT      (MCP4725)
#endif

#ifndef MCP47XX_PARAM_DEV
/** Default I2C device */
#define MCP47XX_PARAM_DEV          I2C_DEV(0)
#endif

#ifndef MCP47XX_PARAM_ADDR
/** Default I2C slave address as offset to MCP47XX_BASE_ADDR */
#define MCP47XX_PARAM_ADDR         (MCP47XX_BASE_ADDR + 2)
#endif

#ifndef MCP47XX_PARAM_GAIN
/** Default MCP47xx gain selection */
#define MCP47XX_PARAM_GAIN         (MCP47XX_GAIN_1X)
#endif

#ifndef MCP47XX_PARAM_VREF
/** Default MCP47xx V_REF selection */
#define MCP47XX_PARAM_VREF         (MCP47XX_VREF_VDD)
#endif

#ifndef MCP47XX_PARAM_PD_MODE
/** Default MCP47xx Power-Down mode selection */
#define MCP47XX_PARAM_PD_MODE      (MCP47XX_PD_LARGE)
#endif

#ifndef MCP47XX_PARAMS
/** Default MCP47xx configuration parameters */
#define MCP47XX_PARAMS { \
                            .dev = MCP47XX_PARAM_DEV, \
                            .addr = MCP47XX_PARAM_ADDR, \
                            .variant = MCP47XX_PARAM_VARIANT, \
                            .gain = MCP47XX_PARAM_GAIN, \
                            .vref = MCP47XX_PARAM_VREF, \
                            .pd_mode = MCP47XX_PARAM_PD_MODE, \
                        },
#endif /* MCP47XX_PARAMS */

#ifndef MCP47XX_SAUL_DAC_PARAMS
/** Example for mapping DAC channels to SAUL */
#define MCP47XX_SAUL_DAC_PARAMS { \
                                    .name = "DAC00", \
                                    .dev = 0, \
                                    .channel = 0, \
                                    .initial = 32768, \
                                },
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const mcp47xx_params_t mcp47xx_params[] =
{
    MCP47XX_PARAMS
};

#if IS_USED(MODULE_SAUL) || DOXYGEN
/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const  mcp47xx_saul_dac_params_t mcp47xx_saul_dac_params[] =
{
    MCP47XX_SAUL_DAC_PARAMS
};
#endif /* IS_USED(MODULE_SAUL) || DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
