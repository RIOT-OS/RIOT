/*
 * SPDX-FileCopyrightText: 2018 HAW-Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for SDS011 particulate matter sensor
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "sds011.h"
#include "sds011_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SDS011_NUM     ARRAY_SIZE(sds011_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static sds011_t sds011_devs[SDS011_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SDS011_NUM];

/**
 * @brief   Define the number of saul info
 */
#define SDS011_INFO_NUM ARRAY_SIZE(sds011_saul_info)

/**
 * @name    Import SAUL endpoint
 * @{
 */
extern const saul_driver_t sds011_saul_driver;
/** @} */

void auto_init_sds011(void)
{
    assert(SDS011_INFO_NUM == SDS011_NUM);

    for (unsigned int i = 0; i < SDS011_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing sds011 #%u\n", i);

        if (sds011_init(&sds011_devs[i], &sds011_params[i]) != SDS011_OK) {
            LOG_ERROR("[auto_init_saul] error initializing sds011 #%u\n", i);
            continue;
        }

        int retries = 0;

        /* sensor must be set to query mode for manual reading by saul */
        while (sds011_set_reporting_mode(&sds011_devs[i], SDS011_RMODE_QUERY) != SDS011_OK) {
            if (retries++ >= 3) {
                LOG_ERROR("[auto_init_saul] error setting sds011 to query mode #%u\n", i);
                continue;
            }
        }

        saul_entries[i].dev = &(sds011_devs[i]);
        saul_entries[i].name = sds011_saul_info[i].name;
        saul_entries[i].driver = &sds011_saul_driver;
        saul_reg_add(&saul_entries[i]);
    }
}
