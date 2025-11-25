/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for HDC1000 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "hdc1000.h"
#include "hdc1000_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define HDC1000_NUM    ARRAY_SIZE(hdc1000_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static hdc1000_t hdc1000_devs[HDC1000_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HDC1000_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define HDC1000_INFO_NUM    ARRAY_SIZE(hdc1000_saul_info)

/**
 * @name    Reference the driver struct
 * @{
 */
extern saul_driver_t hdc1000_saul_temp_driver;
extern saul_driver_t hdc1000_saul_hum_driver;
/** @} */

void auto_init_hdc1000(void)
{
    assert(HDC1000_NUM == HDC1000_INFO_NUM);

    for (unsigned i = 0; i < HDC1000_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hdc1000 #%u\n", i);

        int res = hdc1000_init(&hdc1000_devs[i], &hdc1000_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing hdc1000 #%u\n", i);
            continue;
        }

        saul_entries[(i * 2)].dev = &(hdc1000_devs[i]);
        saul_entries[(i * 2)].name = hdc1000_saul_info[i].name;
        saul_entries[(i * 2)].driver = &hdc1000_saul_temp_driver;
        saul_entries[(i * 2) + 1].dev = &(hdc1000_devs[i]);
        saul_entries[(i * 2) + 1].name = hdc1000_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &hdc1000_saul_hum_driver;
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
