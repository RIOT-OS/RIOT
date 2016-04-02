/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of MMA8652 accelerometer
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 *
 * @}
 */

#ifdef MODULE_MMA8652

#include "saul_reg.h"
#include "mma8652.h"
#include "mma8652_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MMA8652_NUM     (sizeof(mma8652_params)/sizeof(mma8652_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static mma8652_t mma8652_devs[MMA8652_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MMA8652_NUM];

/**
 * @brief   Reference the driver struct
 * @{
 */
extern saul_driver_t mma8652_saul_driver;
/** @} */

void auto_init_mma8652(void)
{
    for (int i = 0; i < MMA8652_NUM; i++) {
        const mma8652_params_t *p = &mma8652_params[i];

        DEBUG("[auto_init_saul] initializing mma8652 acc sensor\n");

        if (mma8652_init(&mma8652_devs[i], p->i2c, p->addr, p->rate, p->scale) < 0) {
            DEBUG("[auto_init_saul] error during initialization\n");
            return;
        }

        if (mma8652_set_active(&mma8652_devs[i]) < 0) {
            DEBUG("[auto_init_saul] error activating mma8652\n");
            return;
        }

        saul_entries[i].dev = &(mma8652_devs[i]);
        saul_entries[i].name = mma8652_saul_info[i].name;
        saul_entries[i].driver = &mma8652_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_MMA8652 */
