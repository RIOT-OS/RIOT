/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of MAG3110 magnetometers
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "mag3110.h"
#include "mag3110_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MAG3110_NUM     ARRAY_SIZE(mag3110_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static mag3110_t mag3110_devs[MAG3110_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MAG3110_NUM];

/**
 * @brief   Define the number of saul info
 */
#define MAG3110_INFO_NUM    ARRAY_SIZE(mag3110_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern const saul_driver_t mag3110_saul_driver;

void auto_init_mag3110(void)
{
    assert(MAG3110_NUM == MAG3110_INFO_NUM);

    for (unsigned i = 0; i < MAG3110_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mag3110 #%u\n", i);

        if (mag3110_init(&mag3110_devs[i], &mag3110_params[i]) != MAG3110_OK) {
            LOG_ERROR("[auto_init_saul] error initializing mag3110 #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(mag3110_devs[i]);
        saul_entries[i].name = mag3110_saul_info[i].name;
        saul_entries[i].driver = &mag3110_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
