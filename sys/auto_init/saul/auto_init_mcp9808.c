/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of MCP9808 driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifdef MODULE_MCP9808

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "mcp9808.h"
#include "mcp9808_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MCP9808_NUM      (sizeof(mcp9808_params) / sizeof(mcp9808_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static mcp9808_t mcp9808_devs[MCP9808_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MCP9808_NUM];

/**
 * @brief   Define the number of saul info
 */
#define MCP9808_INFO_NUM (sizeof(mcp9808_saul_info) / sizeof(mcp9808_saul_info[0]))

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t mcp9808_temperature_saul_driver;
/** @} */

void auto_init_mcp9808(void)
{
    assert(MCP9808_INFO_NUM == MCP9808_NUM);

    for (unsigned i = 0; i < MCP9808_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mcp9808 #%u\n", i);

        if (mcp9808_init(&mcp9808_devs[i],
                         &mcp9808_params[i]) != MCP9808_OK) {
            LOG_ERROR("[auto_init_saul] error initializing mcp9808 #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[i].dev = &(mcp9808_devs[i]);
        saul_entries[i].name = mcp9808_saul_info[i].name;
        saul_entries[i].driver = &mcp9808_temperature_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_MCP9808 */
