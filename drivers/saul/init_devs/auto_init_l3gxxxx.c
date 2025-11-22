/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of ST L3Gxxxx 3-axis gyroscope sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "l3gxxxx.h"
#include "l3gxxxx_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define L3GXXXX_NUM         ARRAY_SIZE(l3gxxxx_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static l3gxxxx_t l3gxxxx_devs[L3GXXXX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[L3GXXXX_NUM];

/**
 * @brief   Define the number of saul info
 */
#define L3GXXXX_INFO_NUM    ARRAY_SIZE(l3gxxxx_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t l3gxxxx_saul_driver;

void auto_init_l3gxxxx(void)
{
    assert(L3GXXXX_NUM == L3GXXXX_INFO_NUM);

    for (unsigned int i = 0; i < L3GXXXX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing l3gd20h #%u\n", i);

        if (l3gxxxx_init(&l3gxxxx_devs[i], &l3gxxxx_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing l3gd20h #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(l3gxxxx_devs[i]);
        saul_entries[i].name = l3gxxxx_saul_info[i].name;
        saul_entries[i].driver = &l3gxxxx_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
