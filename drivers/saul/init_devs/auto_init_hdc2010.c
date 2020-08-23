/*
 * Copyright (C) 2020 Viktor Gal
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
 * @brief       Auto initialization for HDC2010 devices
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "hdc2010.h"
#include "hdc2010_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define HDC2010_NUM    ARRAY_SIZE(hdc2010_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static hdc2010_t hdc2010_devs[HDC2010_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HDC2010_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define HDC2010_INFO_NUM    ARRAY_SIZE(hdc2010_saul_info)

/**
 * @name    Reference the driver struct
 * @{
 */
extern saul_driver_t hdc2010_saul_temp_driver;
extern saul_driver_t hdc2010_saul_hum_driver;
/** @} */


void auto_init_hdc2010(void)
{
    assert(HDC2010_NUM == HDC2010_INFO_NUM);

    for (unsigned i = 0; i < HDC2010_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hdc2010 #%u\n", i);

        int res = hdc2010_init(&hdc2010_devs[i], &hdc2010_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing hdc2010 #%u\n", i);
            continue;
        }

        saul_entries[(i * 2)].dev = &(hdc2010_devs[i]);
        saul_entries[(i * 2)].name = hdc2010_saul_info[i].name;
        saul_entries[(i * 2)].driver = &hdc2010_saul_temp_driver;
        saul_entries[(i * 2) + 1].dev = &(hdc2010_devs[i]);
        saul_entries[(i * 2) + 1].name = hdc2010_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &hdc2010_saul_hum_driver;
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
