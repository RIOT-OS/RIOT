/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for HDC1000 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifdef MODULE_HDC1000

#include "log.h"
#include "saul_reg.h"

#include "hdc1000.h"
#include "hdc1000_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define HDC1000_NUM    (sizeof(hdc1000_params)/sizeof(hdc1000_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static hdc1000_t hdc1000_devs[HDC1000_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HDC1000_NUM * 2];

/**
 * @brief   Reference the driver struct
 * @{
 */
extern saul_driver_t hdc1000_saul_temp_driver;
extern saul_driver_t hdc1000_saul_hum_driver;
/** @} */


void auto_init_hdc1000(void)
{
    for (unsigned i = 0; i < HDC1000_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hdc1000 #%u\n", i);

        int res = hdc1000_init(&hdc1000_devs[i], &hdc1000_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing hdc1000 #%u\n", i);
        }
        else {
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
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_HDC1000 */
