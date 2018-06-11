/*
 * Copyright (C) 2017 HAW Hamburg
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
 * @brief       Auto initialization of TMP006 temperature sensor
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#ifdef MODULE_TMP006

#include "log.h"
#include "saul_reg.h"

#include "tmp006.h"
#include "tmp006_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TMP006_NUM     (sizeof(tmp006_params) / sizeof(tmp006_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static tmp006_t tmp006_devs[TMP006_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TMP006_NUM];

/**
 * @brief   Define the number of saul info
 */
#define TMP006_INFO_NUM    (sizeof(tmp006_saul_info) / sizeof(tmp006_saul_info[0]))

/**
 * @brief   Reference the driver struct
 */
extern const saul_driver_t tmp006_saul_driver;

void auto_init_tmp006(void)
{
    assert(TMP006_NUM == TMP006_INFO_NUM);

    for (unsigned i = 0; i < TMP006_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing tmp006 #%u\n", i);

        if (tmp006_init(&tmp006_devs[i], &tmp006_params[i]) != TMP006_OK) {
            LOG_ERROR("[auto_init_saul] error initializing tmp006 #%u\n", i);
            continue;
        }
        if (tmp006_set_active(&tmp006_devs[i]) != TMP006_OK) {
            LOG_ERROR("[auto_init_saul] error set active tmp006 #%u\n", i);
            continue;
        }
#if TMP006_USE_LOW_POWER
        if (tmp006_set_standby(&tmp006_devs[i]) != TMP006_OK) {
            LOG_ERROR("[auto_init_saul] error set standby tmp006 #%u\n", i);
            continue;
        }
#endif
        saul_entries[i].dev = &(tmp006_devs[i]);
        saul_entries[i].name = tmp006_saul_info[i].name;
        saul_entries[i].driver = &tmp006_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_TMP006 */
