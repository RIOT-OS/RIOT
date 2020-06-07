/*
 * Copyright (C) 2017 HAW Hamburg
 *               2019 HAW Hamburg
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
 * @brief       Auto initialization of TMP00X temperature sensor
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "tmp00x.h"
#include "tmp00x_params.h"
#include "kernel_defines.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TMP00X_NUM     ARRAY_SIZE(tmp00x_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static tmp00x_t tmp00x_devs[TMP00X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TMP00X_NUM];

/**
 * @brief   Define the number of saul info
 */
#define TMP00X_INFO_NUM    ARRAY_SIZE(tmp00x_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern const saul_driver_t tmp00x_saul_driver;

void auto_init_tmp00x(void)
{
    assert(TMP00X_NUM == TMP00X_INFO_NUM);

    for (unsigned i = 0; i < TMP00X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing tmp00x #%u\n", i);

        if (tmp00x_init(&tmp00x_devs[i], &tmp00x_params[i]) != TMP00X_OK) {
            LOG_ERROR("[auto_init_saul] error initializing tmp00x #%u\n", i);
            continue;
        }
        if (tmp00x_set_active(&tmp00x_devs[i]) != TMP00X_OK) {
            LOG_ERROR("[auto_init_saul] error set active tmp00x #%u\n", i);
            continue;
        }
        if (IS_ACTIVE(CONFIG_TMP00X_USE_LOW_POWER)) {
            if (tmp00x_set_standby(&tmp00x_devs[i]) != TMP00X_OK) {
                LOG_ERROR("[auto_init_saul] error set standby tmp00x #%u\n", i);
                continue;
            }
        }
        saul_entries[i].dev = &(tmp00x_devs[i]);
        saul_entries[i].name = tmp00x_saul_info[i].name;
        saul_entries[i].driver = &tmp00x_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
