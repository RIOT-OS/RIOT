/*
 * SPDX-FileCopyrightText: 2016 Kees Bakker, SODAQ
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of BMX280 driver.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "log.h"
#include "saul_reg.h"

#include "bmx280_params.h"
#include "bmx280.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
static bmx280_t bmx280_devs[BMX280_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
#define SENSORS_NUMOF 3
#else
#define SENSORS_NUMOF 2
#endif
static saul_reg_t saul_entries[BMX280_NUMOF * SENSORS_NUMOF];

void auto_init_bmx280(void)
{
    size_t se_ix = 0;
    for (size_t i = 0; i < BMX280_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing BMX280 #%u\n", i);
        int res = bmx280_init(&bmx280_devs[i], &bmx280_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing BMX280 #%i\n", i);
            continue;
        }

        /* temperature */
        saul_entries[se_ix].dev = &bmx280_devs[i];
        saul_entries[se_ix].name = bmx280_saul_reg_info[i].name;
        saul_entries[se_ix].driver = &bmx280_temperature_saul_driver;
        saul_reg_add(&saul_entries[se_ix]);
        se_ix++;

        /* pressure */
        saul_entries[se_ix].dev = &bmx280_devs[i];
        saul_entries[se_ix].name = bmx280_saul_reg_info[i].name;
        saul_entries[se_ix].driver = &bmx280_pressure_saul_driver;
        saul_reg_add(&saul_entries[se_ix]);
        se_ix++;

#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
        /* relative humidity */
        saul_entries[se_ix].dev = &bmx280_devs[i];
        saul_entries[se_ix].name = bmx280_saul_reg_info[i].name;
        saul_entries[se_ix].driver = &bme280_relative_humidity_saul_driver;
        saul_reg_add(&saul_entries[se_ix]);
        se_ix++;
#endif
    }
}
