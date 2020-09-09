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
#include "ads101x.h"
#include "ads101x_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define ADS101X_NUM   ARRAY_SIZE(ads101x_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static ads101x_t ads101x_devs[ADS101X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[ADS101X_NUM];

/**
 * @brief   Define the number of saul info
 */
#define ADS101X_INFO_NUM ARRAY_SIZE(ads101x_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t ads101x_saul_driver;

void auto_init_ads101x(void)
{
    assert(ADS101X_INFO_NUM == ADS101X_NUM);

    for (unsigned i = 0; i < ADS101X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing ads101x #%d\n", i);
        if (ads101x_init(&ads101x_devs[i], &ads101x_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing ads101x #%d\n", i);
            continue;
        }

        saul_entries[i].dev = &(ads101x_devs[i]);
        saul_entries[i].name = ads101x_saul_info[i].name;
        saul_entries[i].driver = &ads101x_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
