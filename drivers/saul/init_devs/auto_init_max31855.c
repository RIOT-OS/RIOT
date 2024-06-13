/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for MAX31855 thermocouple-to-digital converters
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "max31855_params.h"
#include "max31855.h"

/**
 * @brief   Define the number of configured devices
 */
#define MAX31855_NUM     ARRAY_SIZE(max31855_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static max31855_t max31855_devs[MAX31855_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MAX31855_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define MAX31855_INFO_NUM ARRAY_SIZE(max31855_saul_info)

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t max31855_thermo_temp_saul_driver;
extern const saul_driver_t max31855_internal_temp_saul_driver;
/** @} */

void auto_init_max31855(void)
{
    /* each device has thermocouple and internal temperature, needs 2 infos */
    assert(MAX31855_INFO_NUM == 2 * MAX31855_NUM);

    for (unsigned int i = 0; i < MAX31855_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing MAX31855 #%u\n", i);

        if (max31855_init(&max31855_devs[i], &max31855_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing MAX31855 #%u\n", i);
            continue;
        }

        saul_entries[(i * 2)].dev = &(max31855_devs[i]);
        saul_entries[(i * 2)].name = max31855_saul_info[(i * 2)].name;
        saul_entries[(i * 2)].driver = &max31855_thermo_temp_saul_driver;

        saul_entries[(i * 2) + 1].dev = &(max31855_devs[i]);
        saul_entries[(i * 2) + 1].name = max31855_saul_info[(i * 2) + 1].name;
        saul_entries[(i * 2) + 1].driver = &max31855_internal_temp_saul_driver;

        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
