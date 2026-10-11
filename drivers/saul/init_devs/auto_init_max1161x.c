/*
 * SPDX-FileCopyrightText: 2026 Mathis LECRIVAIN <lecrivain.mathis@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of MAX1161X ADC
 *
 * @author      Mathis Lécrivain <lecrivain.mathis@gmail.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "max1161x.h"
#include "max1161x_params.h"
#include "saul_reg.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MAX1161X_NUM ARRAY_SIZE(max1161x_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static max1161x_t max1161x_devs[MAX1161X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MAX1161X_NUM];

/**
 * @brief   Define the number of saul info
 */
#define MAX1161X_INFO_NUM ARRAY_SIZE(max1161x_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t max1161x_saul_driver;

void auto_init_max1161x(void)
{
    assert(MAX1161X_INFO_NUM == MAX1161X_NUM);

    for (unsigned i = 0; i < MAX1161X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing max1161x #%u\n", i);
        if (max1161x_init(&max1161x_devs[i], &max1161x_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing max1161x #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(max1161x_devs[i]);
        saul_entries[i].name = max1161x_saul_info[i].name;
        saul_entries[i].driver = &max1161x_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
