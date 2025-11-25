/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of LIS2DH12 accelerometers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "log.h"
#include "assert.h"
#include "saul_reg.h"
#include "lis2dh12.h"
#include "lis2dh12_params.h"

/**
 * @brief   Number of configured sensors
 */
#define LIS2DH12_NUM    ARRAY_SIZE(lis2dh12_params)

/**
 * @brief   Number of defined SAUL registry info entries
 */
#define LIS2DH12_SAULINFO_NUM   (sizeof(lis2dh12_saul_info) / \
                                 sizeof(lis2dh12_saul_info[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static lis2dh12_t lis2dh12_devs[LIS2DH12_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LIS2DH12_NUM * 2];

void auto_init_lis2dh12(void)
{
    assert(LIS2DH12_NUM == LIS2DH12_SAULINFO_NUM);

    for (unsigned int i = 0; i < LIS2DH12_NUM; i++) {
        int res;

        LOG_DEBUG("[auto_init_saul] initializing lis2dh12 #%u\n", i);

        res = lis2dh12_init(&lis2dh12_devs[i], &lis2dh12_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing lis2dh12 #%u\n", i);
            continue;
        }

        saul_entries[2 * i].dev = &lis2dh12_devs[i];
        saul_entries[2 * i].name = lis2dh12_saul_info[i].name;
        saul_entries[2 * i].driver = &lis2dh12_saul_driver;
        saul_reg_add(&saul_entries[2 * i]);

        saul_entries[2 * i + 1].dev = &lis2dh12_devs[i];
        saul_entries[2 * i + 1].name = lis2dh12_saul_info[i].name;
        saul_entries[2 * i + 1].driver = &lis2dh12_saul_temp_driver;
        saul_reg_add(&saul_entries[2 * i + 1]);
    }
}
