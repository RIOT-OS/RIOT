/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_saul
 * @ingroup     drivers_htu21d
 * @brief       Auto initialization for HTU21D humidity and temperature sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifdef MODULE_HTU21D

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "htu21d_params.h"
#include "htu21d.h"

/**
 * @brief   Define the number of configured sensors
 */
#define HTU21D_NUM     (sizeof(htu21d_params) / sizeof(htu21d_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static htu21d_t htu21d_devs[HTU21D_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HTU21D_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define HTU21D_INFO_NUM (sizeof(htu21d_saul_info) / sizeof(htu21d_saul_info[0]))

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t htu21d_saul_tmp_driver;
extern const saul_driver_t htu21d_saul_hum_driver;
/** @} */

void auto_init_htu21d(void)
{
    assert(HTU21D_INFO_NUM == HTU21D_NUM);

    for (unsigned int i = 0; i < HTU21D_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing htu21d #%u\n", i);

        if (htu21d_init(&htu21d_devs[i], &htu21d_params[i]) != HTU21D_OK) {
            LOG_ERROR("[auto_init_saul] error initializing htu21d #%u\n", i);
            continue;
        }

        saul_entries[(i * 2)].dev = &(htu21d_devs[i]);
        saul_entries[(i * 2)].name = htu21d_saul_info[i].name;
        saul_entries[(i * 2)].driver = &htu21d_saul_tmp_driver;
        saul_entries[(i * 2) + 1].dev = &(htu21d_devs[i]);
        saul_entries[(i * 2) + 1].name = htu21d_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &htu21d_saul_hum_driver;
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_HTU21D */
