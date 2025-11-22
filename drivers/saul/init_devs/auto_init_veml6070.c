/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of VEML6070 driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "veml6070.h"
#include "veml6070_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define VEML6070_NUM    ARRAY_SIZE(veml6070_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static veml6070_t veml6070_devs[VEML6070_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[VEML6070_NUM];

/**
 * @brief   Define the number of saul info
 */
#define VEML6070_INFO_NUM    ARRAY_SIZE(veml6070_saul_info)

/**
 * @brief   Reference the driver structs.
 */
extern const saul_driver_t veml6070_uv_saul_driver;

void auto_init_veml6070(void)
{
    assert(VEML6070_NUM == VEML6070_INFO_NUM);

    for (unsigned i = 0; i < VEML6070_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing veml6070 #%u\n", i);

        if (veml6070_init(&veml6070_devs[i],
                          &veml6070_params[i]) != VEML6070_OK) {
            LOG_ERROR("[auto_init_saul] error initializing veml6070 #%u\n", i);
            continue;
        }

        saul_entries[(i)].dev = &(veml6070_devs[i]);
        saul_entries[(i)].name = veml6070_saul_info[i].name;
        saul_entries[(i)].driver = &veml6070_uv_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i)]));
    }
}
