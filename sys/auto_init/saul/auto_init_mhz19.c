/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
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
 * @brief       Auto initialization of mhz19 driver.
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */

#ifdef MODULE_MHZ19

#include "log.h"
#include "saul_reg.h"

#include "mhz19.h"
#include "mhz19_params.h"

/**
 * @brief  Define the number of configured sensors
 */
#define MHZ19_NUMOF (sizeof(mhz19_params) / sizeof(mhz19_params[0]))

/**
 * @brief  Allocate memory for device descriptors
 */
static mhz19_t mhz19_devs[MHZ19_NUMOF];

/**
 * @brief  Allocate memory for SAUL registry entries
 */
static saul_reg_t saul_entries[MHZ19_NUMOF];

/**
 * @brief  Reference the driver structs.
 * @{
 */
extern const saul_driver_t mhz19_co2_saul_driver;
/** @} */

void auto_init_mhz19(void)
{
    for (unsigned i = 0; i < MHZ19_NUMOF; i++) {
        const mhz19_params_t *p = &mhz19_params[0];

        LOG_DEBUG("[auto_init_saul] initializing mhz19 #%u\n", i);

        if (mhz19_init(&mhz19_devs[i], (mhz19_params_t *)p) != MHZ19_OK) {
            LOG_DEBUG("[auto_init_saul] error initializing mhz19 #%u\n", i);
            return;
        }

        saul_entries[i].dev = &mhz19_devs[i];
        saul_entries[i].name = mhz19_saul_reg_info[i].name;
        saul_entries[i].driver = &mhz19_co2_saul_driver;

        saul_reg_add(&(saul_entries[i]));
    }
}

#endif /* MODULE_MHZ19 */
