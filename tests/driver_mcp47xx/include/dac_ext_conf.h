/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       DAC extension for Microchip MCP47xx DAC with I2C interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef DAC_EXT_CONF_H
#define DAC_EXT_CONF_H

#if MODULE_EXTEND_DAC

#include <stddef.h>

#include "extend/dac.h"
#include "mcp47xx_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to MCP47xx device driver struct
 */
extern const dac_ext_driver_t mcp47xx_extend_dac_driver;
/** @} */

/**
 * @brief   References to the MCP47xx devices
 */
extern mcp47xx_t mcp47xx_dev[];

/**
 * @brief   DAC extension list of MCP47xx devices
 */
static const dac_ext_t dac_ext_list[] =
{
    {
        .driver = &mcp47xx_extend_dac_driver,
        .dev = (void *)&mcp47xx_dev[0],
    },
};

#ifdef __cplusplus
}
#endif

#endif /* MODULE_EXTEND_DAC */

#endif /* DAC_EXT_CONF_H */
/** @} */
