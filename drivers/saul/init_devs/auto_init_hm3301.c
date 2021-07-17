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
 * @brief       Auto initialization for HM3301 particle matter sensor
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "hm3301_params.h"
#include "hm3301.h"

/**
 * @brief   Allocate memory for the device descriptors
 */
static hm3301_t hm3301_devs[HM3301_NUMOF];

#if IS_ACTIVE(MODULE_SAUL)
/**
 * @brief   Number of logical saul devices per physical sensor
 */
#define HM3301_SAUL_DEV_NUM      (6)

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HM3301_NUMOF * HM3301_SAUL_DEV_NUM];

/**
 * @brief   Define the number of saul info
 */
#define HM3301_INFO_NUM ARRAY_SIZE(hm3301_saul_info)

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t hm3301_saul_driver_mc_pm_1;
extern const saul_driver_t hm3301_saul_driver_mc_pm_2p5;
extern const saul_driver_t hm3301_saul_driver_mc_pm_10;
extern const saul_driver_t hm3301_saul_driver_nc_pm_1;
extern const saul_driver_t hm3301_saul_driver_nc_pm_2p5;
extern const saul_driver_t hm3301_saul_driver_nc_pm_10;
/** @} */
#endif

void auto_init_hm3301(void)
{
#if IS_ACTIVE(MODULE_SAUL)
    assert(HM3301_INFO_NUM == HM3301_NUMOF);
#endif

    for (unsigned int i = 0; i < HM3301_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hm3301 #%u\n", i);

        if (hm3301_init(&hm3301_devs[i], &hm3301_params[i])) {
            LOG_ERROR("[auto_init_saul] error initializing hm3301 #%u\n",
                      i);
            continue;
        }
    #if IS_ACTIVE(MODULE_SAUL)
        saul_entries[(i * HM3301_SAUL_DEV_NUM)].driver = &hm3301_saul_driver_mc_pm_1;
        saul_entries[(i * HM3301_SAUL_DEV_NUM) + 1].driver = &hm3301_saul_driver_mc_pm_2p5;
        saul_entries[(i * HM3301_SAUL_DEV_NUM) + 2].driver = &hm3301_saul_driver_mc_pm_10;
        saul_entries[(i * HM3301_SAUL_DEV_NUM) + 3].driver = &hm3301_saul_driver_nc_pm_1;
        saul_entries[(i * HM3301_SAUL_DEV_NUM) + 4].driver = &hm3301_saul_driver_nc_pm_2p5;
        saul_entries[(i * HM3301_SAUL_DEV_NUM) + 5].driver = &hm3301_saul_driver_nc_pm_10;
        /* the physical device is the same for all logical SAUL instances */
        for (unsigned x = 0; x < HM3301_SAUL_DEV_NUM; x++) {
            saul_entries[i * HM3301_SAUL_DEV_NUM + x].dev = &(hm3301_devs[i]);
            saul_entries[i * HM3301_SAUL_DEV_NUM + x].name = hm3301_saul_info[i].name;
            saul_reg_add(&saul_entries[i * HM3301_SAUL_DEV_NUM + x]);
        }
    #endif
    }
}
