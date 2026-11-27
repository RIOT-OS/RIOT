/*
 * SPDX-FileCopyrightText: 2026 UGA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of TMP117 temperature sensor
 *
 * @author      léo cordier <leo.cordier@univ-grenoble-alpes.fr>
 *
 * @}
 */
#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "container.h"

#include "tmp117.h"
#include "tmp117_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TMP117_NUM     ARRAY_SIZE(tmp117_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static tmp117_t tmp117_devs[TMP117_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TMP117_NUM];

/**
 * @brief   Define the number of SAUL info
 */
#define TMP117_INFO_NUM     ARRAY_SIZE(tmp117_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern const saul_driver_t tmp117_saul_driver;

void auto_init_tmp117(void)
{
    assert(TMP117_NUM == TMP117_INFO_NUM);

    for (unsigned i = 0; i < TMP117_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing TMP117 #%u\n", i);

        if (tmp117_init(&tmp117_devs[i], &tmp117_params[i]) != TMP117_OK) {
            LOG_ERROR("[auto_init_saul] error initializing TMP117 #%u\n", i);
            continue;
        }
        saul_entries[i].dev = &(tmp117_devs[i]);
        saul_entries[i].name = tmp117_saul_info[i].name;
        saul_entries[i].driver = &tmp117_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
