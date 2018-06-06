/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
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

#ifdef MODULE_LIS2DH12

#include "log.h"
#include "assert.h"
#include "saul_reg.h"
#include "lis2dh12.h"
#include "lis2dh12_params.h"

/**
 * @brief   Number of configured sensors
 */
#define LIS2DH12_NUM    (sizeof(lis2dh12_params) / sizeof(lis2dh12_params[0]))


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
static saul_reg_t saul_entries[LIS2DH12_NUM];

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

        saul_entries[i].dev = &(lis2dh12_devs[i]);
        saul_entries[i].name = lis2dh12_saul_info[i].name;
        saul_entries[i].driver = &lis2dh12_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_LIS2DH12 */
