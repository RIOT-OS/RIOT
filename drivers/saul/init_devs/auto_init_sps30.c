/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of Sensirion SPS30 device driver
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @file
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "sps30.h"
#include "sps30_params.h"

/**
 * @brief   Number of logical saul devices per physical sensor
 */
#define SPS30_SAUL_DEV_NUM      (5)

/**
 * @brief   Allocation of memory for device descriptors
 */
sps30_t sps30_devs[SPS30_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SPS30_NUM * SPS30_SAUL_DEV_NUM];

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t sps30_saul_driver_mc_pm_1_2p5_4;
extern const saul_driver_t sps30_saul_driver_mc_pm_10;
extern const saul_driver_t sps30_saul_driver_nc_pm_0p5_1_2p5;
extern const saul_driver_t sps30_saul_driver_nc_pm_4_10;
extern const saul_driver_t sps30_saul_driver_ps;
/** @} */

void auto_init_sps30(void)
{
    assert(SPS30_INFO_NUM == SPS30_NUM);

    for (unsigned i = 0; i < SPS30_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing sps30 #%u\n", i);

        if (sps30_init(&sps30_devs[i],
                       &sps30_params[i]) != SPS30_OK) {
            LOG_ERROR("[auto_init_saul] error initializing sps30 #%u\n", i);
            continue;
        }

        saul_entries[i * 5].driver = &sps30_saul_driver_mc_pm_1_2p5_4;
        saul_entries[i * 5 + 1].driver = &sps30_saul_driver_mc_pm_10;
        saul_entries[i * 5 + 2].driver = &sps30_saul_driver_nc_pm_0p5_1_2p5;
        saul_entries[i * 5 + 3].driver = &sps30_saul_driver_nc_pm_4_10;
        saul_entries[i * 5 + 4].driver = &sps30_saul_driver_ps;

        /* the physical device is the same for all logical SAUL instances */
        for (unsigned x = 0; x < SPS30_SAUL_DEV_NUM; x++) {
            saul_entries[i * 5 + x].dev = &(sps30_devs[i]);
            saul_entries[i * 5 + x].name = sps30_saul_info[i].name;
            saul_reg_add(&saul_entries[i * 5 + x]);
        }
    }
}
