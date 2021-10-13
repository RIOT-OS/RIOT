/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Auto initialization of Si70xx driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "si70xx.h"
#include "si70xx_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SI70XX_NUM    ARRAY_SIZE(si70xx_params)

/**
 * @brief   Define the number of SAUL registry entries
 */
#if SI70XX_HAS_HUMIDITY_SENSOR
#define SI70XX_SAUL_ENTRIES_NUM 2
#else
#define SI70XX_SAUL_ENTRIES_NUM 1
#endif

/**
 * @brief   Allocation of memory for device descriptors
 */
static si70xx_t si70xx_devs[SI70XX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SI70XX_NUM * SI70XX_SAUL_ENTRIES_NUM];

/**
 * @brief   Define the number of saul info
 */
#define SI70XX_INFO_NUM    ARRAY_SIZE(si70xx_saul_info)

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t si70xx_temperature_saul_driver;
extern const saul_driver_t si70xx_relative_humidity_saul_driver;
/** @} */

void auto_init_si70xx(void)
{
    assert(SI70XX_INFO_NUM == SI70XX_NUM);

    unsigned entry = 0;
    for (unsigned i = 0; i < SI70XX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing SI70xx #%u\n", i);

        if (si70xx_init(&si70xx_devs[i], &si70xx_params[i]) != SI70XX_OK) {
            LOG_ERROR("[auto_init_saul] error initializing SI70xx #%i\n", i);
            continue;
        }

        /* temperature */
        saul_entries[entry].dev = &si70xx_devs[i];
        saul_entries[entry].name = si70xx_saul_info[i].name;
        saul_entries[entry].driver = &si70xx_temperature_saul_driver;
        saul_reg_add(&saul_entries[entry++]);

#if SI70XX_HAS_HUMIDITY_SENSOR
        /* relative humidity */
        saul_entries[entry].dev = &si70xx_devs[i];
        saul_entries[entry].name = si70xx_saul_info[i].name;
        saul_entries[entry].driver = &si70xx_relative_humidity_saul_driver;
        saul_reg_add(&saul_entries[entry++]);
#endif /* SI70XX_HAS_HUMIDITY_SENSOR */
    }
}
