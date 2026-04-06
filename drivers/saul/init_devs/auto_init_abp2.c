/*
 * SPDX-FileCopyrightText: 2024 CNRS, France
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of Honeywell ABP2 series
 *              pressure and temperature sensor
 *
 * @author      David Picard <david.picard@clermont.in2p3.fr>
 * @}
 */

#include "container.h"
#include "log.h"
#include "saul_reg.h"
#include "abp2.h"
#include "abp2_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define ABP2_NUM          ARRAY_SIZE(abp2_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static abp2_t abp2_devs[ABP2_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[ABP2_NUM * 2];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t abp2_saul_driver_press;
extern saul_driver_t abp2_saul_driver_temp;

void auto_init_abp2(void)
{
    for (unsigned int i = 0; i < ABP2_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing abp2 #%u\n", i);

        int res = abp2_init(&abp2_devs[i], &abp2_params[i]);
        if (res) {
            LOG_ERROR("[auto_init_saul] error initializing abp2 #%u\n", i);
            continue;
        }

        /* pressure */
        saul_entries[(i * 2)].dev = &(abp2_devs[i]);
        saul_entries[(i * 2)].name = abp2_saul_info[i][0].name;
        saul_entries[(i * 2)].driver = &abp2_saul_driver_press;
        saul_reg_add(&(saul_entries[(i * 2)]));

        /* temperature */
        saul_entries[(i * 2) + 1].dev = &(abp2_devs[i]);
        saul_entries[(i * 2) + 1].name = abp2_saul_info[i][1].name;
        saul_entries[(i * 2) + 1].driver = &abp2_saul_driver_temp;
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
