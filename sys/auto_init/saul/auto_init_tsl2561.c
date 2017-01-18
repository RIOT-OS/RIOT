/*
 * Copyright (C) 2016 Inria
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
 * @brief       Auto initialization of TSL2561 driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifdef MODULE_TSL2561

#include "log.h"
#include "saul_reg.h"

#include "tsl2561_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TSL2561_NUMOF    (sizeof(tsl2561_params) / sizeof(tsl2561_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static tsl2561_t tsl2561_devs[TSL2561_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TSL2561_NUMOF];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t tsl2561_illuminance_saul_driver;
/** @} */

void auto_init_tsl2561(void)
{
    for (unsigned i = 0; i < TSL2561_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing tsl2561 #%u\n", i);

        if (tsl2561_init(&tsl2561_devs[i],
                         tsl2561_params[i].i2c_dev,
                         tsl2561_params[i].addr,
                         tsl2561_params[i].gain,
                         tsl2561_params[i].integration) != TSL2561_OK) {
            LOG_ERROR("[auto_init_saul] error initializing tsl2561 #%u\n", i);
            return;
        }

        /* illuminance */
        saul_entries[i].dev = &(tsl2561_devs[i]);
        saul_entries[i].name = tsl2561_saul_reg_info[i].name;
        saul_entries[i].driver = &tsl2561_illuminance_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_TSL2561 */
