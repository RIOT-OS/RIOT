/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of MH-Z19 C02 sensor device.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "mhz19.h"
#include "mhz19_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MHZ19_NUM   ARRAY_SIZE(mhz19_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static mhz19_t mhz19_devs[MHZ19_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MHZ19_NUM];

/**
 * @brief   Define the number of saul info
 */
#define MHZ19_INFO_NUM    ARRAY_SIZE(mhz19_saul_info)

/**
 * @brief   Reference the driver structs.
 */
extern const saul_driver_t mhz19_ppm_saul_driver;

void auto_init_mhz19(void)
{
    assert(MHZ19_NUM == MHZ19_INFO_NUM);

    for (unsigned i = 0; i < MHZ19_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mhz19 #%u\n", i);

        if (mhz19_init(&mhz19_devs[i], &mhz19_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing mhz19 #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(mhz19_devs[i]);
        saul_entries[i].name = mhz19_saul_info[i].name;
        saul_entries[i].driver = &mhz19_ppm_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
