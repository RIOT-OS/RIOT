/*
 * SPDX-FileCopyrightText: 2024 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of ws281x RGB LED devices
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "ws281x.h"
#include "ws281x_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define WS281X_NUM     ARRAY_SIZE(ws281x_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static ws281x_t ws281x_devs[WS281X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[WS281X_NUM];

/**
 * @brief   Define the number of saul info
 */
#define WS281X_INFO_NUM ARRAY_SIZE(ws281x_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern const saul_driver_t ws281x_saul_driver;

void auto_init_ws281x(void)
{
    assert(WS281X_NUM == WS281X_INFO_NUM);

    for (unsigned i = 0; i < WS281X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing WS281x #%u: ", i);

        if (ws281x_init(&ws281x_devs[i], &ws281x_params[i]) != 0) {
            LOG_ERROR("ERROR\n");
            continue;
        }
        LOG_DEBUG("SUCCESS\n");
        saul_entries[i].dev = &(ws281x_devs[i]);
        saul_entries[i].name = ws281x_saul_info[i].name;
        saul_entries[i].driver = &ws281x_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
