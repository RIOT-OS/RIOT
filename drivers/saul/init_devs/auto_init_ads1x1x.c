/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2018 Acutam Automation, LLC
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
 * @brief       Auto initialization of ADS101x/111x ADCs
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"

#include "saul_reg.h"
#include "ads1x1x.h"
#include "ads1x1x_params.h"
#include "container.h"

/**
 * @brief   Define the number of configured sensors
 */
#define ADS1X1X_NUM   ARRAY_SIZE(ads1x1x_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static ads1x1x_t ads1x1x_devs[ADS1X1X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[ADS1X1X_NUM];

/**
 * @brief   Define the number of saul info
 */
#define ADS1X1X_INFO_NUM ARRAY_SIZE(ads1x1x_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t ads1x1x_saul_driver;

void auto_init_ads1x1x(void)
{
    assert(ADS1X1X_INFO_NUM == ADS1X1X_NUM);

    for (unsigned i = 0; i < ADS1X1X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing ads1x1x #%d\n", i);
        if (ads1x1x_init(&ads1x1x_devs[i], &ads1x1x_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing ads1x1x #%d\n", i);
            continue;
        }

        saul_entries[i].dev = &(ads1x1x_devs[i]);
        saul_entries[i].name = ads1x1x_saul_info[i].name;
        saul_entries[i].driver = &ads1x1x_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
