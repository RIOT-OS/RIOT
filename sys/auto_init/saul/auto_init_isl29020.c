/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     auto_init_saul
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

#include "saul_reg.h"
#include "isl29020.h"
#include "isl29020_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define ISL29020_NUM    (sizeof(isl29020_params)/sizeof(isl29020_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static isl29020_t isl29020_devs[ISL29020_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[ISL29020_NUM];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t isl29020_saul_driver;


void auto_init_isl29020(void)
{
    for (int i = 0; i < ISL29020_NUM; i++) {
        const isl29020_params_t *p = &isl29020_params[i];

        DEBUG("[auto_init_saul] initializing isl29020 light sensor\n");
        int res = isl29020_init(&isl29020_devs[i], p->i2c, p->addr,
                                p->range, p->mode);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during initialization\n");
        }
        else {
            saul_entries[i].dev = &(isl29020_devs[i]);
            saul_entries[i].name = isl29020_saul_info[i].name;
            saul_entries[i].driver = &isl29020_saul_driver;
            saul_reg_add(&(saul_entries[i]));
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ISL29020 */
