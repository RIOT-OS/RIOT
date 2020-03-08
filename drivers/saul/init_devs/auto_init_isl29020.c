/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Auto initialization of ISL29020 light sensors
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifdef MODULE_ISL29020

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "isl29020.h"
#include "isl29020_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define ISL29020_NUM    ARRAY_SIZE(isl29020_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static isl29020_t isl29020_devs[ISL29020_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[ISL29020_NUM];

/**
 * @brief   Define the number of saul info
 */
#define ISL29020_INFO_NUM    ARRAY_SIZE(isl29020_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t isl29020_saul_driver;


void auto_init_isl29020(void)
{
    assert(ISL29020_NUM == ISL29020_INFO_NUM);

    for (unsigned int i = 0; i < ISL29020_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing isl29020 #%u\n", i);

        int res = isl29020_init(&isl29020_devs[i], &isl29020_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing isl29020 #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(isl29020_devs[i]);
        saul_entries[i].name = isl29020_saul_info[i].name;
        saul_entries[i].driver = &isl29020_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ISL29020 */
