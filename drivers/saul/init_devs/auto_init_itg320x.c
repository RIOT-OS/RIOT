/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_itg320x
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of InvenSense ITG320X 3-axis gyroscope
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "itg320x.h"
#include "itg320x_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define ITG320X_NUM    (ARRAY_SIZE(itg320x_params))

/**
 * @brief   Allocate memory for the device descriptors
 */
static itg320x_t itg320x_devs[ITG320X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[ITG320X_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define ITG320X_INFO_NUM    (ARRAY_SIZE(itg320x_saul_info))

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t itg320x_saul_gyro_driver;
extern saul_driver_t itg320x_saul_temp_driver;
/** @} */

void auto_init_itg320x(void)
{
    assert(ITG320X_NUM == ITG320X_INFO_NUM);

    for (unsigned int i = 0; i < ITG320X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing itg320x #%u\n", i);

        if (itg320x_init(&itg320x_devs[i], &itg320x_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing itg320x #%u\n", i);
            continue;
        }

        saul_entries[(i * 2)].dev = &(itg320x_devs[i]);
        saul_entries[(i * 2)].name = itg320x_saul_info[i].name;
        saul_entries[(i * 2)].driver = &itg320x_saul_gyro_driver;
        saul_entries[(i * 2) + 1].dev = &(itg320x_devs[i]);
        saul_entries[(i * 2) + 1].name = itg320x_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &itg320x_saul_temp_driver;
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
