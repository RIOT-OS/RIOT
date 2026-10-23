/*
 * SPDX-FileCopyrightText: 2026 Anastasiia
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of LIS331DLH accelerometers
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "lis331dlh.h"
#include "lis331dlh_params.h"

#define LIS331DLH_NUM       ARRAY_SIZE(lis331dlh_params)
#define LIS331DLH_INFO_NUM  ARRAY_SIZE(lis331dlh_saul_info)

static lis331dlh_t lis331dlh_devs[LIS331DLH_NUM];
static saul_reg_t saul_entries[LIS331DLH_NUM];

extern const saul_driver_t lis331dlh_saul_driver;

void auto_init_lis331dlh(void)
{
    assert(LIS331DLH_NUM == LIS331DLH_INFO_NUM);

    for (unsigned int i = 0; i < LIS331DLH_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing lis331dlh #%u\n", i);

        if (lis331dlh_init(&lis331dlh_devs[i], &lis331dlh_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing lis331dlh #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(lis331dlh_devs[i]);
        saul_entries[i].name = lis331dlh_saul_info[i].name;
        saul_entries[i].driver = &lis331dlh_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
