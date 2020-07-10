/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of L3GD20H 3-axis gyroscope sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifdef MODULE_L3GD20H

#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "l3gd20h.h"
#include "l3gd20h_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define L3GD20H_NUM    (sizeof(l3gd20h_params) / sizeof(l3gd20h_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static l3gd20h_t l3gd20h_devs[L3GD20H_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[L3GD20H_NUM];

/**
 * @brief   Define the number of saul info
 */
#define L3GD20H_INFO_NUM    (sizeof(l3gd20h_saul_info) / sizeof(l3gd20h_saul_info[0]))

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t l3gd20h_saul_driver;


void auto_init_l3gd20h(void)
{
    assert(L3GD20H_NUM == L3GD20H_INFO_NUM);

    for (unsigned int i = 0; i < L3GD20H_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing l3gd20h #%u\n", i);

        if (l3gd20h_init(&l3gd20h_devs[i], &l3gd20h_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing l3gd20h #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(l3gd20h_devs[i]);
        saul_entries[i].name = l3gd20h_saul_info[i].name;
        saul_entries[i].driver = &l3gd20h_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_L3GD20H */
