/*
 * SPDX-FileCopyrightText: 2020 Locha Inc
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of GP2Y10xx ADC
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"

#include "saul_reg.h"
#include "gp2y10xx.h"
#include "gp2y10xx_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define GP2Y10XX_NUM    ARRAY_SIZE(gp2y10xx_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static gp2y10xx_t gp2y10xx_devs[GP2Y10XX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[GP2Y10XX_NUM];

/**
 * @brief   Define the number of saul info
 */
#define  GP2Y10XX_INFO_NUM ARRAY_SIZE(gp2y10xx_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t gp2y10xx_saul_driver;

void auto_init_gp2y10xx(void)
{
    assert(GP2Y10XX_INFO_NUM == GP2Y10XX_NUM);

    for (unsigned i = 0; i < GP2Y10XX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing gp2y10xx #%d\n", i);
        if (gp2y10xx_init(&gp2y10xx_devs[i], &gp2y10xx_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing gp2y10xx #%d\n", i);
            continue;
        }

        saul_entries[i].dev = &(gp2y10xx_devs[i]);
        saul_entries[i].name = gp2y10xx_saul_info[i].name;
        saul_entries[i].driver = &gp2y10xx_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
