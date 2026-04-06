/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mcp47xx
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of Microchip MCP47xx DAC with I2C interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include "assert.h"
#include "log.h"

#include "saul_reg.h"
#include "saul/periph.h"

#include "mcp47xx.h"
#include "mcp47xx_params.h"

/**
 * @brief   Number of configured MCP47xx DAC devices
 */
#define MCP47XX_NUM    (sizeof(mcp47xx_params) / sizeof(mcp47xx_params[0]))

/**
 * @brief   Number of configured SAUL MCP47xx DAC channels
 */
#define MCP47XX_SAUL_DAC_NUMOF (sizeof(mcp47xx_saul_dac_params) / \
                                sizeof(mcp47xx_saul_dac_params[0]))

/**
 * @brief   Number of saul info
 */
#define MCP47XX_INFO_NUM    (sizeof(mcp47xx_saul_info) / \
                             sizeof(mcp47xx_saul_info[0]))

/**
 * @brief   Allocate the memory for the MCP47xx DAC device descriptors
 */
mcp47xx_t mcp47xx_devs[MCP47XX_NUM];

/**
 * @brief   Allocate the memory for MCP47xx DAC SAUL registry entries
 */
static saul_reg_t mcp47xx_saul_reg_entries[MCP47XX_SAUL_DAC_NUMOF];

/**
 * @brief   Reference the MCP47xx DAC input mode driver struct
 */
extern saul_driver_t mcp47xx_dac_in_saul_driver;

/**
 * @brief   Reference to the MCP47xx DAC output mode driver struct
 */
extern saul_driver_t mcp47xx_dac_saul_driver;

void auto_init_mcp47xx(void)
{
    for (unsigned int i = 0; i < MCP47XX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing MCP47xx DAC dev #%u\n", i);
        mcp47xx_init(&mcp47xx_devs[i], &mcp47xx_params[i]);
    }

    for (unsigned int i = 0; i < MCP47XX_SAUL_DAC_NUMOF; i++) {
        const mcp47xx_saul_dac_params_t *p = &mcp47xx_saul_dac_params[i];

        LOG_DEBUG("[auto_init_saul] initializing MCP47xx DAC channel #%u\n", i);

        /* check the MCP47xx device index */
        assert(p->dev < MCP47XX_NUM);

        mcp47xx_saul_reg_entries[i].dev = (void *)p;
        mcp47xx_saul_reg_entries[i].name = p->name;
        mcp47xx_saul_reg_entries[i].driver = &mcp47xx_dac_saul_driver;

        /* initialize the MCP47xx pin */
        mcp47xx_dac_init(&mcp47xx_devs[p->dev], p->channel);
        mcp47xx_dac_set(&mcp47xx_devs[p->dev], p->channel, p->initial);

        /* add to registry */
        saul_reg_add(&(mcp47xx_saul_reg_entries[i]));
    }
}
