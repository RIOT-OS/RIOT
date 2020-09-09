/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of LIS3MDL magnetometer
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "lis3mdl.h"
#include "lis3mdl_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LIS3MDL_NUM    ARRAY_SIZE(lis3mdl_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static lis3mdl_t lis3mdl_devs[LIS3MDL_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LIS3MDL_NUM];

/**
 * @brief   Define the number of saul info
 */
#define LIS3MDL_INFO_NUM    ARRAY_SIZE(lis3mdl_saul_info)

/**
 * @brief   Reference the driver structs
 */
extern saul_driver_t lis3mdl_saul_mag_driver;

void auto_init_lis3mdl(void)
{
    assert(LIS3MDL_NUM == LIS3MDL_INFO_NUM);

    for (unsigned int i = 0; i < LIS3MDL_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing lis3mdl #%u\n", i);

        if (lis3mdl_init(&lis3mdl_devs[i], &lis3mdl_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing lis3mdl #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(lis3mdl_devs[i]);
        saul_entries[i].name = lis3mdl_saul_info[i].name;
        saul_entries[i].driver = &lis3mdl_saul_mag_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
