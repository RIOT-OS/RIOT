/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of Sensirion SHT30/SHT31/SHT35 device driver
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "sht3x.h"
#include "sht3x_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SHT3X_NUM      ARRAY_SIZE(sht3x_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
sht3x_dev_t sht3x_devs[SHT3X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SHT3X_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define SHT3X_INFO_NUM ARRAY_SIZE(sht3x_saul_info)

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t sht3x_saul_driver_temperature;
extern const saul_driver_t sht3x_saul_driver_humidity;
/** @} */

void auto_init_sht3x(void)
{
    assert(SHT3X_INFO_NUM == SHT3X_NUM);

    for (unsigned i = 0; i < SHT3X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing sht3x #%u\n", i);

        if (sht3x_init(&sht3x_devs[i],
                       &sht3x_params[i]) != SHT3X_OK) {
            LOG_ERROR("[auto_init_saul] error initializing sht3x #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[(i * 2)].dev = &(sht3x_devs[i]);
        saul_entries[(i * 2)].name = sht3x_saul_info[i].name;
        saul_entries[(i * 2)].driver = &sht3x_saul_driver_temperature;

        /* relative humidity */
        saul_entries[(i * 2) + 1].dev = &(sht3x_devs[i]);
        saul_entries[(i * 2) + 1].name = sht3x_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &sht3x_saul_driver_humidity;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
