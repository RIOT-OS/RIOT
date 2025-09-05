/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     drivers_vl6180x
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of ST VL6180X Ranging and Ambient Light Sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifdef MODULE_VL6180X

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "vl6180x.h"
#include "vl6180x_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define VL6180X_NUM    ARRAY_SIZE(vl6180x_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static vl6180x_t vl6180x_devs[VL6180X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[VL6180X_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define VL6180X_INFO_NUM ARRAY_SIZE(vl6180x_saul_info)

/**
 * @brief   Reference the driver structs
 */
extern saul_driver_t vl6180x_saul_als_driver;
extern saul_driver_t vl6180x_saul_rng_driver;

void auto_init_vl6180x(void)
{
    assert(VL6180X_INFO_NUM == VL6180X_NUM);

    for (unsigned i = 0; i < VL6180X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing vl6180x #%u\n", i);

        if (vl6180x_init(&vl6180x_devs[i], &vl6180x_params[i]) != VL6180X_OK) {
            LOG_ERROR("[auto_init_saul] error initializing vl6180x #%u\n", i);
            continue;
        }

        saul_entries[(i * 2)].dev = &(vl6180x_devs[i]);
        saul_entries[(i * 2)].name = vl6180x_saul_info[i].name;
        saul_entries[(i * 2)].driver = &vl6180x_saul_als_driver;
        saul_reg_add(&(saul_entries[(i * 2)]));

        saul_entries[(i * 2) + 1].dev = &(vl6180x_devs[i]);
        saul_entries[(i * 2) + 1].name = vl6180x_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &vl6180x_saul_rng_driver;
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_VL6180X */
