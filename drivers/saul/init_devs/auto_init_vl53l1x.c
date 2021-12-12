/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     drivers_vl53l1x
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of ST VL53L1X Time-of-Flight (ToF) ranging sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifdef MODULE_VL53L1X

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "vl53l1x.h"
#include "vl53l1x_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define VL53L1X_NUM    (sizeof(vl53l1x_params) / sizeof(vl53l1x_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static vl53l1x_t vl53l1x_devs[VL53L1X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[VL53L1X_NUM];

/**
 * @brief   Define the number of saul info
 */
#define VL53L1X_INFO_NUM (sizeof(vl53l1x_saul_info) / sizeof(vl53l1x_saul_info[0]))

/**
 * @brief   Reference the driver structs
 */
extern saul_driver_t vl53l1x_saul_driver;

void auto_init_vl53l1x(void)
{
    assert(VL53L1X_INFO_NUM == VL53L1X_NUM);

    for (unsigned i = 0; i < VL53L1X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing vl53l1x #%u\n", i);

        if (vl53l1x_init(&vl53l1x_devs[i], &vl53l1x_params[i]) != VL53L1X_OK) {
            LOG_ERROR("[auto_init_saul] error initializing vl53l1x #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(vl53l1x_devs[i]);
        saul_entries[i].name = vl53l1x_saul_info[i].name;
        saul_entries[i].driver = &vl53l1x_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_VL53L1X */
