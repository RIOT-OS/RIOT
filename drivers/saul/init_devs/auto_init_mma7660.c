/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of MMA7660 accelerometers
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "mma7660.h"
#include "mma7660_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MMA7660_NUM       ARRAY_SIZE(mma7660_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static mma7660_t mma7660_devs[MMA7660_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MMA7660_NUM];

/**
 * @brief   Define the number of configured sensors
 */
#define MMA7660_INFO_NUM  ARRAY_SIZE(mma7660_saul_info)

/**
 * @brief   Reference the driver struct
 * @{
 */
extern saul_driver_t mma7660_saul_driver;
/** @} */

void auto_init_mma7660(void)
{
    assert(MMA7660_NUM == MMA7660_INFO_NUM);

    for (unsigned i = 0; i < MMA7660_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mma7660 #%u\n", i);

        if (mma7660_init(&mma7660_devs[i], &mma7660_params[i]) != MMA7660_OK) {
            LOG_ERROR("[auto_init_saul] error initializing mma7660 #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(mma7660_devs[i]);
        saul_entries[i].name = mma7660_saul_info[i].name;
        saul_entries[i].driver = &mma7660_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
