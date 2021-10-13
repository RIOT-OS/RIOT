/*
 * Copyright (C) 2017-2018 Inria
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
 * @brief       Auto initialization of Si114x driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *              Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "si114x.h"
#include "si114x_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SI114X_NUMOF    ARRAY_SIZE(si114x_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static si114x_t si114x_devs[SI114X_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SI114X_NUMOF * 4];

/**
 * @brief   Define the number of saul info
 */
#define SI114X_INFO_NUMOF    ARRAY_SIZE(si114x_saul_reg_info)

/**
 * @name    Reference the driver structs
 * @{
 */
extern const saul_driver_t si114x_uv_saul_driver;
extern const saul_driver_t si114x_ir_saul_driver;
extern const saul_driver_t si114x_visible_saul_driver;
extern const saul_driver_t si114x_distance_saul_driver;
/** @} */

void auto_init_si114x(void)
{
    assert(SI114X_INFO_NUMOF == SI114X_NUMOF);

    for (unsigned i = 0; i < SI114X_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing Si114x #%u\n", i);

        if (si114x_init(&si114x_devs[i], &si114x_params[i]) != SI114X_OK) {
            LOG_ERROR("[auto_init_saul] error initializing Si114x #%i\n", i);
            continue;
        }

        /* UV index */
        saul_entries[i * 4].dev = &si114x_devs[i];
        saul_entries[i * 4].name = si114x_saul_reg_info[i].name;
        saul_entries[i * 4].driver = &si114x_uv_saul_driver;

        /* Infra red */
        saul_entries[(i * 4) + 1].dev = &si114x_devs[i];
        saul_entries[(i * 4) + 1].name = si114x_saul_reg_info[i].name;
        saul_entries[(i * 4) + 1].driver = &si114x_ir_saul_driver;

        /* Visible */
        saul_entries[(i * 4) + 2].dev = &si114x_devs[i];
        saul_entries[(i * 4) + 2].name = si114x_saul_reg_info[i].name;
        saul_entries[(i * 4) + 2].driver = &si114x_visible_saul_driver;

        /* Distance */
        saul_entries[(i * 4) + 3].dev = &si114x_devs[i];
        saul_entries[(i * 4) + 3].name = si114x_saul_reg_info[i].name;
        saul_entries[(i * 4) + 3].driver = &si114x_distance_saul_driver;

        saul_reg_add(&saul_entries[i * 4]);
        saul_reg_add(&saul_entries[(i * 4) + 1]);
        saul_reg_add(&saul_entries[(i * 4) + 2]);
        saul_reg_add(&saul_entries[(i * 4) + 3]);
    }
}
