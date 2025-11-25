/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of AMS CCS811 digital gas sensor driver
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "ccs811.h"
#include "ccs811_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define CCS811_NUM      ARRAY_SIZE(ccs811_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static ccs811_t ccs811_devs[CCS811_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[CCS811_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define CCS811_INFO_NUM ARRAY_SIZE(ccs811_saul_info)

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t ccs811_saul_driver_eco2;
extern const saul_driver_t ccs811_saul_driver_tvoc;
/** @} */

void auto_init_ccs811(void)
{
    assert(CCS811_INFO_NUM == CCS811_NUM);

    for (unsigned i = 0; i < CCS811_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing ccs811 #%u\n", i);

        if (ccs811_init(&ccs811_devs[i], &ccs811_params[i]) != CCS811_OK) {
            LOG_ERROR("[auto_init_saul] error initializing ccs811 #%u\n", i);
            continue;
        }

        /* eCO2 */
        saul_entries[(i * 2)].dev = &(ccs811_devs[i]);
        saul_entries[(i * 2)].name = ccs811_saul_info[i].name;
        saul_entries[(i * 2)].driver = &ccs811_saul_driver_eco2;

        /* TVOC */
        saul_entries[(i * 2) + 1].dev = &(ccs811_devs[i]);
        saul_entries[(i * 2) + 1].name = ccs811_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &ccs811_saul_driver_tvoc;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
