/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of Bosch BME680 device driver
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "bme680.h"
#include "bme680_params.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
bme680_t bme680_devs_saul[BME680_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[BME680_NUMOF * 4];

/**
 * @brief   Define the number of saul info
 */
#define BME680_INFO_NUMOF   ARRAY_SIZE(bme680_saul_info)

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t bme680_saul_driver_temperature;
extern const saul_driver_t bme680_saul_driver_pressure;
extern const saul_driver_t bme680_saul_driver_humidity;
extern const saul_driver_t bme680_saul_driver_gas;
/** @} */

void auto_init_bme680(void)
{
    assert(BME680_INFO_NUMOF == BME680_NUMOF);

    for (unsigned i = 0; i < BME680_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing BME680 #%u\n", i);

        if (bme680_init(&bme680_devs_saul[i],
                        &bme680_params[i]) != BME680_OK) {
            LOG_ERROR("[auto_init_saul] error initializing BME680 #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[(i * 4)].dev = &(bme680_devs_saul[i]);
        saul_entries[(i * 4)].name = bme680_saul_info[i].name;
        saul_entries[(i * 4)].driver = &bme680_saul_driver_temperature;

        /* pressure */
        saul_entries[(i * 4) + 1].dev = &(bme680_devs_saul[i]);
        saul_entries[(i * 4) + 1].name = bme680_saul_info[i].name;
        saul_entries[(i * 4) + 1].driver = &bme680_saul_driver_pressure;

        /* relative humidity */
        saul_entries[(i * 4) + 2].dev = &(bme680_devs_saul[i]);
        saul_entries[(i * 4) + 2].name = bme680_saul_info[i].name;
        saul_entries[(i * 4) + 2].driver = &bme680_saul_driver_humidity;

        /* relative humidity */
        saul_entries[(i * 4) + 3].dev = &(bme680_devs_saul[i]);
        saul_entries[(i * 4) + 3].name = bme680_saul_info[i].name;
        saul_entries[(i * 4) + 3].driver = &bme680_saul_driver_gas;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i * 4)]));
        saul_reg_add(&(saul_entries[(i * 4) + 1]));
        saul_reg_add(&(saul_entries[(i * 4) + 2]));
        saul_reg_add(&(saul_entries[(i * 4) + 3]));
    }
}
