/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#if defined(MODULE_BME280) || defined(MODULE_BMP280)

#include "log.h"
#include "saul_reg.h"

#include "bmx280_params.h"
#include "bmx280.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
static bmx280_t bmx280_devs[BMX280_NUMOF];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t bmx280_temperature_saul_driver;
extern const saul_driver_t bmx280_pressure_saul_driver;
#if defined(MODULE_BME280)
extern const saul_driver_t bme280_relative_humidity_saul_driver;
#endif
/** @} */

/**
 * @brief   Memory for the SAUL registry entries
 */
#if defined(MODULE_BME280)
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

#if defined(MODULE_BME280)
        /* relative humidity */
        saul_entries[se_ix].dev = &bmx280_devs[i];
        saul_entries[se_ix].name = bmx280_saul_reg_info[i].name;
        saul_entries[se_ix].driver = &bme280_relative_humidity_saul_driver;
        saul_reg_add(&saul_entries[se_ix]);
        se_ix++;
#endif
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_BMX280 */
