/*
 * Copyright (C) 2017 Frits Kuipers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of ds18 driver.
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 *
 * @}
 */

#include "log.h"
#include "saul_reg.h"

#include "ds18.h"
#include "ds18_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define DS18_NUMOF    ARRAY_SIZE(ds18_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static ds18_t ds18_devs[DS18_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[DS18_NUMOF];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t ds18_temperature_saul_driver;
/** @} */

void auto_init_ds18(void)
{
    for (unsigned i = 0; i < DS18_NUMOF; i++) {
        const ds18_params_t *p = &ds18_params[i];

        LOG_DEBUG("[auto_init_saul] initializing ds18 #%u\n", i);

        if (ds18_init(&ds18_devs[i], p) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing ds18 #%u\n", i);
            return;
        }

        /* temperature */
        saul_entries[i].dev = &(ds18_devs[i]);
        saul_entries[i].name = ds18_saul_reg_info[i].name;
        saul_entries[i].driver = &ds18_temperature_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
