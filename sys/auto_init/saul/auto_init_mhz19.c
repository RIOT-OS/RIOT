/*
 * Copyright (C) 2018 Koen Zandberg
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
 * @brief       Auto initialization of MH-Z19 C02 sensor device.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifdef MODULE_MHZ19

#include "log.h"
#include "saul_reg.h"
#include "mhz19.h"
#include "mhz19_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MHZ19_NUMOF    (sizeof(mhz19_params) / sizeof(mhz19_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static mhz19_t mhz19_devs[MHZ19_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MHZ19_NUMOF];

/**
 * @brief   Define the number of saul info
 */
#define MHZ19_INFO_NUM    (sizeof(mhz19_saul_info) / sizeof(mhz19_saul_info[0]))

/**
 * @brief   Reference the driver structs.
 */
extern const saul_driver_t mhz19_ppm_saul_driver;

void auto_init_mhz19(void)
{
    assert(MHZ19_NUM == MHZ19_INFO_NUM);

    for (unsigned i = 0; i < MHZ19_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mhz19 #%u\n", i);

        if (mhz19_init(&mhz19_devs[i], &mhz19_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing mhz19 #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(mhz19_devs[i]);
        saul_entries[i].name = mhz19_saul_reg_info[i].name;
        saul_entries[i].driver = &mhz19_ppm_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_MHZ19 */
