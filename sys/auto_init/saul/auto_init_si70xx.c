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

#ifdef MODULE_SI70XX

#include "log.h"
#include "saul_reg.h"
#include "si70xx.h"
#include "si70xx_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SI70XX_NUM    (sizeof(si70xx_params) / sizeof(si70xx_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static si70xx_t si70xx_devs[SI70XX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SI70XX_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define SI70XX_INFO_NUM    (sizeof(si70xx_saul_info) / sizeof(si70xx_saul_info[0]))

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

    for (unsigned i = 0; i < SI70XX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing SI70xx #%u\n", i);

        if (si70xx_init(&si70xx_devs[i], &si70xx_params[i]) != SI70XX_OK) {
            LOG_ERROR("[auto_init_saul] error initializing SI70xx #%i\n", i);
            continue;
        }

        /* temperature */
        saul_entries[i * 2].dev = &si70xx_devs[i];
        saul_entries[i * 2].name = si70xx_saul_info[i].name;
        saul_entries[i * 2].driver = &si70xx_temperature_saul_driver;

        /* relative humidity */
        saul_entries[(i * 2) + 1].dev = &si70xx_devs[i];
        saul_entries[(i * 2) + 1].name = si70xx_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = \
                &si70xx_relative_humidity_saul_driver;

        saul_reg_add(&saul_entries[i * 2]);
        saul_reg_add(&saul_entries[(i * 2) + 1]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SI70XX */
