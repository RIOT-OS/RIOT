/*
 * SPDX-FileCopyrightText: 2020 Viktor Gal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for Adafruit Seesaw Soil devices
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "seesaw_soil.h"
#include "seesaw_soil_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SEESAW_SOIL_NUM    ARRAY_SIZE(seesaw_soil_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static seesaw_soil_t seesaw_soil_devs[SEESAW_SOIL_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SEESAW_SOIL_NUM];

/**
 * @brief   Define the number of saul info
 */
#define SEESAW_SOIL_INFO_NUM    ARRAY_SIZE(seesaw_soil_saul_info)

/**
 * @name    Reference the driver struct
 * @{
 */
extern saul_driver_t seesaw_soil_saul_temp_driver;
/** @} */

void auto_init_seesaw_soil(void)
{
    assert(SEESAW_SOIL_NUM == SEESAW_SOIL_INFO_NUM);

    for (unsigned i = 0; i < SEESAW_SOIL_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing seesaw_soil #%u\n", i);

        int res = seesaw_soil_init(&seesaw_soil_devs[i], &seesaw_soil_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing seesaw_soil #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(seesaw_soil_devs[i]);
        saul_entries[i].name = seesaw_soil_saul_info[i].name;
        saul_entries[i].driver = &seesaw_soil_saul_temp_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
