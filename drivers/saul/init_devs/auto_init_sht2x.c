/*
 * Copyright (C) 2019 Kees Bakker, SODAQ
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
 * @brief       Auto initialization of SHT2X driver.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 *
 * @}
 */

#include "log.h"
#include "saul_reg.h"

#include "sht2x_params.h"
#include "sht2x.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
static sht2x_t sht2x_devs[SHT2X_NUMOF];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t sht2x_temperature_saul_driver;
extern const saul_driver_t sht2x_relative_humidity_saul_driver;
/** @} */

/**
 * @brief   Memory for the SAUL registry entries
 */
#define SENSORS_NUMOF 2
static saul_reg_t saul_entries[SHT2X_NUMOF * SENSORS_NUMOF];

void auto_init_sht2x(void)
{
    size_t se_ix = 0;
    for (size_t i = 0; i < SHT2X_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing SHT2X #%u\n", i);
        int res = sht2x_init(&sht2x_devs[i], &sht2x_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing SHT2X #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[se_ix].dev = &sht2x_devs[i];
        saul_entries[se_ix].name = sht2x_saul_reg_info[i].name;
        saul_entries[se_ix].driver = &sht2x_temperature_saul_driver;
        saul_reg_add(&saul_entries[se_ix]);
        se_ix++;

        /* relative humidity */
        saul_entries[se_ix].dev = &sht2x_devs[i];
        saul_entries[se_ix].name = sht2x_saul_reg_info[i].name;
        saul_entries[se_ix].driver = &sht2x_relative_humidity_saul_driver;
        saul_reg_add(&saul_entries[se_ix]);
        se_ix++;
    }
}
