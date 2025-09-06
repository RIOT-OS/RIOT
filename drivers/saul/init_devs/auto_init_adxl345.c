/*
 * SPDX-FileCopyrightText: 2017 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of ADXL345 accelerometer
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "adxl345.h"
#include "adxl345_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define ADXL345_NUM    ARRAY_SIZE(adxl345_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static adxl345_t adxl345_devs[ADXL345_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[ADXL345_NUM];

/**
 * @brief   Define the number of saul info
 */
#define ADXL345_INFO_NUM ARRAY_SIZE(adxl345_saul_info)

/**
 * @brief   Reference the driver structs
 */
extern saul_driver_t adxl345_saul_driver;

void auto_init_adxl345(void)
{
    assert(ADXL345_INFO_NUM == ADXL345_NUM);

    for (unsigned i = 0; i < ADXL345_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing adxl345 #%u\n", i);

        if (adxl345_init(&adxl345_devs[i], &adxl345_params[i]) != ADXL345_OK) {
            LOG_ERROR("[auto_init_saul] error initializing adxl345 #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(adxl345_devs[i]);
        saul_entries[i].name = adxl345_saul_info[i].name;
        saul_entries[i].driver = &adxl345_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
