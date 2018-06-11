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
 * @brief       Auto initialization of Grove LED bar
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#ifdef MODULE_GROVE_LEDBAR

#include "log.h"
#include "saul_reg.h"

#include "grove_ledbar.h"
#include "grove_ledbar_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define GROVE_LEDBAR_NUM     (sizeof(grove_ledbar_params) / sizeof(grove_ledbar_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static grove_ledbar_t grove_ledbar_devs[GROVE_LEDBAR_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[GROVE_LEDBAR_NUM];

/**
 * @brief   Define the number of saul info
 */
#define GROVE_LEDBAR_INFO_NUM (sizeof(grove_ledbar_saul_info) / sizeof(grove_ledbar_saul_info[0]))

/**
 * @brief   Reference the driver struct
 */
extern const saul_driver_t grove_ledbar_saul_driver;

void auto_init_grove_ledbar(void)
{
    assert(GROVE_LEDBAR_NUM == GROVE_LEDBAR_INFO_NUM);

    for (unsigned i = 0; i < GROVE_LEDBAR_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing Grove LED bar #%u: ", i);

        if (my9221_init(&grove_ledbar_devs[i], &grove_ledbar_params[i]) != MY9221_OK) {
            LOG_ERROR("ERROR\n");
            continue;
        }
        LOG_DEBUG("SUCCESS\n");
        saul_entries[i].dev = &(grove_ledbar_devs[i]);
        saul_entries[i].name = grove_ledbar_saul_info[i].name;
        saul_entries[i].driver = &grove_ledbar_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_GROVE_LEDBAR */
