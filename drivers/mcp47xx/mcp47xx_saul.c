/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mcp47xx
 * @brief       MCP47xx adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */
#if MODULE_SAUL

#include <string.h>

#include "saul.h"
#include "mcp47xx.h"

extern mcp47xx_t mcp47xx_devs[];

static int set(const void *dev, const phydat_t *data)
{
    const mcp47xx_saul_dac_params_t *p = (const mcp47xx_saul_dac_params_t *)dev;
    mcp47xx_dac_set(&mcp47xx_devs[p->dev], p->channel, (uint16_t)data->val[0]);
    return 1;
}

static int get(const void *dev, phydat_t *data)
{
    const mcp47xx_saul_dac_params_t *p = (const mcp47xx_saul_dac_params_t *)dev;
    mcp47xx_dac_get(&mcp47xx_devs[p->dev], p->channel, (uint16_t*)&data->val[0]);
    return 1;
}

const saul_driver_t mcp47xx_dac_saul_driver = {
    .read = get,
    .write = set,
    .type = SAUL_ACT_DIMMER
};
#endif /* MODULE_SAUL */
