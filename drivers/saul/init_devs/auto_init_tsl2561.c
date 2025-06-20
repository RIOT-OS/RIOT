/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of TSL2561 driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "tsl2561.h"
#include "tsl2561_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TSL2561_NUMOF    ARRAY_SIZE(tsl2561_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static tsl2561_t tsl2561_devs[TSL2561_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TSL2561_NUMOF];

/**
 * @brief   Define the number of saul info
 */
#define TSL2561_INFO_NUMOF    ARRAY_SIZE(tsl2561_saul_info)

/**
 * @brief   Reference the driver structs.
 */
extern const saul_driver_t tsl2561_illuminance_saul_driver;

void auto_init_tsl2561(void)
{
    assert(TSL2561_NUMOF == TSL2561_INFO_NUMOF);

    for (unsigned i = 0; i < TSL2561_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing tsl2561 #%u\n", i);

        if (tsl2561_init(&tsl2561_devs[i],
                         &tsl2561_params[i]) != TSL2561_OK) {
            LOG_ERROR("[auto_init_saul] error initializing tsl2561 #%u\n", i);
            continue;
        }

        /* illuminance */
        saul_entries[i].dev = &(tsl2561_devs[i]);
        saul_entries[i].name = tsl2561_saul_info[i].name;
        saul_entries[i].driver = &tsl2561_illuminance_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
