/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of lm75 compatible driver.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "lm75.h"
#include "lm75_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LM75_NUMOF      ARRAY_SIZE(lm75_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static lm75_t lm75_devs[LM75_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LM75_NUMOF];

/**
 * @brief   Reference the driver structs.
 */
extern const saul_driver_t lm75_temperature_saul_driver;

void auto_init_lm75(void)
{
    for (unsigned i = 0; i < LM75_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing lm75 #%u\n", i);

        if (lm75_init(&lm75_devs[i], &lm75_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing lm75 #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[i].dev = &lm75_devs[i];
        saul_entries[i].name = "lm75";
        saul_entries[i].driver = &lm75_temperature_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
