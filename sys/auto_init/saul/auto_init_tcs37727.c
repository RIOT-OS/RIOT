/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief       Auto initialization of TCS37727 light sensors
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifdef MODULE_TCS37727

#include "log.h"
#include "saul_reg.h"
#include "tcs37727.h"
#include "tcs37727_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TCS37727_NUM    (sizeof(tcs37727_params) / sizeof(tcs37727_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static tcs37727_t tcs37727_devs[TCS37727_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TCS37727_NUM];

/**
 * @brief   Define the number of saul info
 */
#define TCS37727_INFO_NUM    (sizeof(tcs37727_saul_info) / sizeof(tcs37727_saul_info[0]))

/**
 * @brief   Export the sensor's SAUL interface
 */
extern const saul_driver_t tcs37727_saul_driver;

void auto_init_tcs37727(void)
{
    assert(TCS37727_NUM == TCS37727_INFO_NUM);

    for (unsigned i = 0; i < TCS37727_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing tcs29020 #%u\n", i);

        if (tcs37727_init(&tcs37727_devs[i],
                          &tcs37727_params[i]) != TCS37727_OK) {
            LOG_ERROR("[auto_init_saul] error initializing tcs37727 #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(tcs37727_devs[i]);
        saul_entries[i].name = tcs37727_saul_info[i].name;
        saul_entries[i].driver = &tcs37727_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_TCS37727 */
