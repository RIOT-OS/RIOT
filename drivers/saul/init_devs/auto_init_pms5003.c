/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 * @file
 * @brief       Auto initialization for PMS5003 dust sensor
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "pms5003_params.h"
#include "pms5003.h"

/**
 * @brief   Allocate memory for the device descriptors
 */
static pms5003_t pms5003_devs[PMS5003_NUMOF];

#if IS_ACTIVE(MODULE_SAUL)
/**
 * @brief   Number of logical saul devices per physical sensor
 */
#define PMS5003_SAUL_DEV_NUM      (6)

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[PMS5003_NUMOF * PMS5003_SAUL_DEV_NUM];

/**
 * @brief   Define the number of saul info
 */
#define PMS5003_INFO_NUM ARRAY_SIZE(pms5003_saul_info)

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t pms5003_saul_driver_mc_pm_1;
extern const saul_driver_t pms5003_saul_driver_mc_pm_2p5;
extern const saul_driver_t pms5003_saul_driver_mc_pm_10;
extern const saul_driver_t pms5003_saul_driver_nc_pm_1;
extern const saul_driver_t pms5003_saul_driver_nc_pm_2p5;
extern const saul_driver_t pms5003_saul_driver_nc_pm_10;
/** @} */
#endif

void auto_init_pms5003(void)
{
#if IS_ACTIVE(MODULE_SAUL)
    assert(PMS5003_INFO_NUM == PMS5003_NUMOF);
#endif

    for (unsigned int i = 0; i < PMS5003_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing pms5003 #%u\n", i);

        if (pms5003_init(&pms5003_devs[i], &pms5003_params[i])) {
            LOG_ERROR("[auto_init_saul] error initializing pms5003 #%u\n",
                      i);
            continue;
        }
    #if IS_ACTIVE(MODULE_SAUL)
        saul_entries[(i * PMS5003_SAUL_DEV_NUM)].driver = &pms5003_saul_driver_mc_pm_1;
        saul_entries[(i * PMS5003_SAUL_DEV_NUM) + 1].driver = &pms5003_saul_driver_mc_pm_2p5;
        saul_entries[(i * PMS5003_SAUL_DEV_NUM) + 2].driver = &pms5003_saul_driver_mc_pm_10;
        saul_entries[(i * PMS5003_SAUL_DEV_NUM) + 3].driver = &pms5003_saul_driver_nc_pm_1;
        saul_entries[(i * PMS5003_SAUL_DEV_NUM) + 4].driver = &pms5003_saul_driver_nc_pm_2p5;
        saul_entries[(i * PMS5003_SAUL_DEV_NUM) + 5].driver = &pms5003_saul_driver_nc_pm_10;
        /* the physical device is the same for all logical SAUL instances */
        for (unsigned x = 0; x < PMS5003_SAUL_DEV_NUM; x++) {
            saul_entries[i * PMS5003_SAUL_DEV_NUM + x].dev = &(pms5003_devs[i]);
            saul_entries[i * PMS5003_SAUL_DEV_NUM + x].name = pms5003_saul_info[i].name;
            saul_reg_add(&saul_entries[i * PMS5003_SAUL_DEV_NUM + x]);
        }
    #endif
    }
}
