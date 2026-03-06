/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of MPL3115A2 pressure sensor
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "mpl3115a2.h"
#include "mpl3115a2_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MPL3115A2_NUM     ARRAY_SIZE(mpl3115a2_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static mpl3115a2_t mpl3115a2_devs[MPL3115A2_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MPL3115A2_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define MPL3115A2_INFO_NUM    ARRAY_SIZE(mpl3115a2_saul_info)

/**
 * @name    Reference the driver struct
 * @{
 */
extern const saul_driver_t mpl3115a2_pressure_saul_driver;
extern const saul_driver_t mpl3115a2_temperature_saul_driver;
/** @} */

void auto_init_mpl3115a2(void)
{
    assert(MPL3115A2_NUM == MPL3115A2_INFO_NUM);

    for (unsigned i = 0; i < MPL3115A2_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mpl3115a2 #%u\n", i);

        if ((mpl3115a2_init(&mpl3115a2_devs[i], &mpl3115a2_params[i]) |
             mpl3115a2_set_active(&mpl3115a2_devs[i])) != MPL3115A2_OK) {
            LOG_ERROR("[auto_init_saul] error initializing mpl3115a2 #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[(i * 2)].dev = &(mpl3115a2_devs[i]);
        saul_entries[(i * 2)].name = mpl3115a2_saul_info[i].name;
        saul_entries[(i * 2)].driver = &mpl3115a2_temperature_saul_driver;

        /* atmospheric pressure */
        saul_entries[(i * 2) + 1].dev = &(mpl3115a2_devs[i]);
        saul_entries[(i * 2) + 1].name = mpl3115a2_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &mpl3115a2_pressure_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
