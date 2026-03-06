/*
 * SPDX-FileCopyrightText: 2018 UC Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for PIR devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include "log.h"
#include "saul_reg.h"
#include "pir_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define PIR_NUM    ARRAY_SIZE(pir_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static pir_t pir_devs[PIR_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[PIR_NUM];

/**
 * @brief   Reference to the occupancy driver struct
 * @{
 */
extern saul_driver_t pir_saul_occup_driver;
/** @} */

void auto_init_pir(void)
{
    for (unsigned i = 0; i < PIR_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing pir #%u\n", i);

        int res = pir_init(&pir_devs[i], &pir_params[i]);
        if (res != 0) {
            LOG_ERROR("[auto_init_saul] error initializing pir #%u\n", i);
        }
        else {
            saul_entries[i].dev = &(pir_devs[i]);
            saul_entries[i].name = pir_saul_info[i].name;
            saul_entries[i].driver = &pir_saul_occup_driver;
            saul_reg_add(&(saul_entries[i]));
        }
    }
}
