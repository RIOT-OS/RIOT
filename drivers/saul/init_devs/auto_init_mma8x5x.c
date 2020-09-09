/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @brief       Auto initialization of MMA8x5x accelerometers
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "mma8x5x.h"
#include "mma8x5x_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MMA8X5X_NUM     ARRAY_SIZE(mma8x5x_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static mma8x5x_t mma8x5x_devs[MMA8X5X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MMA8X5X_NUM];

/**
 * @brief   Define the number of saul info
 */
#define MMA8X5X_INFO_NUM    ARRAY_SIZE(mma8x5x_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t mma8x5x_saul_driver;

void auto_init_mma8x5x(void)
{
    assert(MMA8X5X_NUM == MMA8X5X_INFO_NUM);

    for (unsigned i = 0; i < MMA8X5X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mma8x5x #%u\n", i);

        if (mma8x5x_init(&mma8x5x_devs[i], &mma8x5x_params[i]) != MMA8X5X_OK) {
            LOG_ERROR("[auto_init_saul] error initializing mma8x5x #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(mma8x5x_devs[i]);
        saul_entries[i].name = mma8x5x_saul_info[i].name;
        saul_entries[i].driver = &mma8x5x_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
