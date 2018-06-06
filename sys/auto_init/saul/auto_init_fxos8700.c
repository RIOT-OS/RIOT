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
 * @brief       Auto initialization for FXOS8700 devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#ifdef MODULE_FXOS8700

#include "log.h"
#include "saul_reg.h"
#include "fxos8700_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define FXOS8700_NUM    (sizeof(fxos8700_params)/sizeof(fxos8700_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static fxos8700_t fxos8700_devs[FXOS8700_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[FXOS8700_NUM * 2];

/**
 * @name    Reference the driver struct
 * @{
 */
extern saul_driver_t fxos8700_saul_acc_driver;
extern saul_driver_t fxos8700_saul_mag_driver;
/** @} */


void auto_init_fxos8700(void)
{
    for (unsigned i = 0; i < FXOS8700_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing fxos8700 #%u\n", i);

        int res = fxos8700_init(&fxos8700_devs[i], &fxos8700_params[i]);
        if (res != 0) {
            LOG_ERROR("[auto_init_saul] error initializing fxos8700 #%u\n", i);
        }
        else {
            saul_entries[(i * 2)].dev = &(fxos8700_devs[i]);
            saul_entries[(i * 2)].name = fxos8700_saul_info[i].name;
            saul_entries[(i * 2)].driver = &fxos8700_saul_acc_driver;
            saul_entries[(i * 2) + 1].dev = &(fxos8700_devs[i]);
            saul_entries[(i * 2) + 1].name = fxos8700_saul_info[i].name;
            saul_entries[(i * 2) + 1].driver = &fxos8700_saul_mag_driver;
            saul_reg_add(&(saul_entries[(i * 2)]));
            saul_reg_add(&(saul_entries[(i * 2) + 1]));
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_FXOS8700 */
