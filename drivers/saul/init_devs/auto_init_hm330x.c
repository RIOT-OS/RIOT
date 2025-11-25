/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 * @file
 * @brief       Auto initialization for HM330X particle matter sensor
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "hm330x_params.h"
#include "hm330x.h"

/**
 * @brief   Allocate memory for the device descriptors
 */
static hm330x_t hm330x_devs[HM330X_NUMOF];

/**
 * @brief   Number of logical saul devices per physical sensor
 */
#if IS_USED(MODULE_HM3302)
#define HM330X_SAUL_DEV_NUM      (6)
#else
#define HM330X_SAUL_DEV_NUM      (3)
#endif

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HM330X_NUMOF * HM330X_SAUL_DEV_NUM];

/**
 * @brief   Define the number of saul info
 */
#define HM330X_INFO_NUM ARRAY_SIZE(hm330x_saul_info)

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t hm330x_saul_driver_mc_pm_1;
extern const saul_driver_t hm330x_saul_driver_mc_pm_2p5;
extern const saul_driver_t hm330x_saul_driver_mc_pm_10;
#if IS_USED(MODULE_HM3302)
extern const saul_driver_t hm330x_saul_driver_nc_pm_1;
extern const saul_driver_t hm330x_saul_driver_nc_pm_2p5;
extern const saul_driver_t hm330x_saul_driver_nc_pm_10;
#endif
/** @} */

void auto_init_hm330x(void)
{
    assert(HM330X_INFO_NUM == HM330X_NUMOF);

    for (unsigned int i = 0; i < HM330X_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hm330x #%u\n", i);

        if (hm330x_init(&hm330x_devs[i], &hm330x_params[i])) {
            LOG_ERROR("[auto_init_saul] error initializing hm330x #%u\n",
                      i);
            continue;
        }
        saul_entries[(i * HM330X_SAUL_DEV_NUM)].driver = &hm330x_saul_driver_mc_pm_1;
        saul_entries[(i * HM330X_SAUL_DEV_NUM) + 1].driver = &hm330x_saul_driver_mc_pm_2p5;
        saul_entries[(i * HM330X_SAUL_DEV_NUM) + 2].driver = &hm330x_saul_driver_mc_pm_10;
#if IS_USED(MODULE_HM3302)
        saul_entries[(i * HM330X_SAUL_DEV_NUM) + 3].driver = &hm330x_saul_driver_nc_pm_1;
        saul_entries[(i * HM330X_SAUL_DEV_NUM) + 4].driver = &hm330x_saul_driver_nc_pm_2p5;
        saul_entries[(i * HM330X_SAUL_DEV_NUM) + 5].driver = &hm330x_saul_driver_nc_pm_10;
#endif
        /* the physical device is the same for all logical SAUL instances */
        for (unsigned x = 0; x < HM330X_SAUL_DEV_NUM; x++) {
            saul_entries[i * HM330X_SAUL_DEV_NUM + x].dev = &(hm330x_devs[i]);
            saul_entries[i * HM330X_SAUL_DEV_NUM + x].name = hm330x_saul_info[i].name;
            saul_reg_add(&saul_entries[i * HM330X_SAUL_DEV_NUM + x]);
        }
    }
}
