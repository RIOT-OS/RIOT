/*
 * Copyright (C) 2017 Inria
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
 * @brief       Auto initialization of VEML6070 driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifdef MODULE_VEML6070

#include "log.h"
#include "saul_reg.h"

#include "veml6070_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define VEML6070_NUMOF    (sizeof(veml6070_params) / sizeof(veml6070_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static veml6070_t veml6070_devs[VEML6070_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[VEML6070_NUMOF];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t veml6070_uv_saul_driver;
/** @} */

void auto_init_veml6070(void)
{
    for (unsigned i = 0; i < VEML6070_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing veml6070 #%u\n", i);

        if (veml6070_init(&veml6070_devs[i],
                          &veml6070_params[i]) != VEML6070_OK) {
            LOG_ERROR("[auto_init_saul] error initializing veml6070 #%u\n", i);
            return;
        }

        saul_entries[(i)].dev = &(veml6070_devs[i]);
        saul_entries[(i)].name = veml6070_saul_reg_info[i].name;
        saul_entries[(i)].driver = &veml6070_uv_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i)]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_VEML6070 */
