/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#if IS_USED(MODULE_SAUL)
#include "saul_reg.h"
#endif

#include "sgp30.h"
#include "sgp30_params.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
static sgp30_t sgp30_devs[SGP30_NUM];

#if IS_USED(MODULE_SAUL)
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
#endif

void auto_init_sgp30(void)
{
    assert(SGP30_INFO_NUM == SGP30_NUM);

    for (unsigned i = 0; i < SGP30_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing sgp30 #%u\n", i);

        if (sgp30_init(&sgp30_devs[i], &sgp30_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing sgp30 #%u\n", i);
            continue;
        }

#if IS_USED(MODULE_SAUL)
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
#endif
    }
}
