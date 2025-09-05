/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of AMS SGP30 digital gas sensor driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @file
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "sgp30.h"
#include "sgp30_params.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
static sgp30_t sgp30_devs[SGP30_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SGP30_NUM * 2];

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t sgp30_saul_driver_eco2;
extern const saul_driver_t sgp30_saul_driver_tvoc;
/** @} */

void auto_init_sgp30(void)
{
    assert(SGP30_INFO_NUM == SGP30_NUM);

    for (unsigned i = 0; i < SGP30_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing sgp30 #%u\n", i);

        if (sgp30_init(&sgp30_devs[i], &sgp30_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing sgp30 #%u\n", i);
            continue;
        }

        /* eCO2 */
        saul_entries[(i * 2)].dev = &(sgp30_devs[i]);
        saul_entries[(i * 2)].name = sgp30_saul_info[i].name;
        saul_entries[(i * 2)].driver = &sgp30_saul_driver_eco2;

        /* TVOC */
        saul_entries[(i * 2) + 1].dev = &(sgp30_devs[i]);
        saul_entries[(i * 2) + 1].name = sgp30_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &sgp30_saul_driver_tvoc;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
