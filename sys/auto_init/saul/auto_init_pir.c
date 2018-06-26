/*
 * Copyright (C) 2018 UC Berkeley
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
 * @brief       Auto initialization for PIR devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#ifdef MODULE_PIR

#include "log.h"
#include "saul_reg.h"
#include "pir_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define PIR_NUM    (sizeof(pir_params)/sizeof(pir_params[0]))

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

#else
typedef int dont_be_pedantic;
#endif /* MODULE_PIR */
