/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of LSM303DLHC accelerometer/magnetometer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "lsm303dlhc.h"
#include "lsm303dlhc_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LSM303DLHC_NUM    ARRAY_SIZE(lsm303dlhc_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static lsm303dlhc_t lsm303dlhc_devs[LSM303DLHC_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LSM303DLHC_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define LSM303DLHC_INFO_NUM    ARRAY_SIZE(lsm303dlhc_saul_info)

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t lsm303dlhc_saul_acc_driver;
extern saul_driver_t lsm303dlhc_saul_mag_driver;
/** @} */

void auto_init_lsm303dlhc(void)
{
    assert(LSM303DLHC_NUM == LSM303DLHC_INFO_NUM);

    for (unsigned int i = 0; i < LSM303DLHC_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing lsm303dlhc #%u\n", i);

        if (lsm303dlhc_init(&lsm303dlhc_devs[i], &lsm303dlhc_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing lsm303dlhc #%u\n", i);
            continue;
        }

        saul_entries[(i * 2)].dev = &(lsm303dlhc_devs[i]);
        saul_entries[(i * 2)].name = lsm303dlhc_saul_info[i].name;
        saul_entries[(i * 2)].driver = &lsm303dlhc_saul_acc_driver;
        saul_entries[(i * 2) + 1].dev = &(lsm303dlhc_devs[i]);
        saul_entries[(i * 2) + 1].name = lsm303dlhc_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &lsm303dlhc_saul_mag_driver;
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
