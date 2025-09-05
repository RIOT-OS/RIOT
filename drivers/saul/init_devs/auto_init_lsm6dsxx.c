/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of LSM6DSXX accelerometer/gyroscope sensors
 *
 * @author      Vincent Dupont <vincent@otakeys.com
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "lsm6dsxx.h"
#include "lsm6dsxx_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LSM6DSXX_NUM    ARRAY_SIZE(lsm6dsxx_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static lsm6dsxx_t lsm6dsxx_devs[LSM6DSXX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LSM6DSXX_NUM * 3];

/**
 * @brief   Define the number of saul info
 */
#define LSM6DSXX_INFO_NUM    ARRAY_SIZE(lsm6dsxx_saul_info)

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t lsm6dsxx_saul_acc_driver;
extern saul_driver_t lsm6dsxx_saul_gyro_driver;
extern saul_driver_t lsm6dsxx_saul_temp_driver;
/** @} */

void auto_init_lsm6dsxx(void)
{
    assert(LSM6DSXX_NUM == LSM6DSXX_INFO_NUM);

    for (unsigned int i = 0; i < LSM6DSXX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing lsm6dsxx #%u\n", i);

        if (lsm6dsxx_init(&lsm6dsxx_devs[i], &lsm6dsxx_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing lsm6dsxx #%u\n", i);
            continue;
        }

        saul_entries[(i * 3)].dev = &(lsm6dsxx_devs[i]);
        saul_entries[(i * 3)].name = lsm6dsxx_saul_info[i].name;
        saul_entries[(i * 3)].driver = &lsm6dsxx_saul_acc_driver;
        saul_reg_add(&(saul_entries[(i * 3)]));
        saul_entries[(i * 3) + 1].dev = &(lsm6dsxx_devs[i]);
        saul_entries[(i * 3) + 1].name = lsm6dsxx_saul_info[i].name;
        saul_entries[(i * 3) + 1].driver = &lsm6dsxx_saul_gyro_driver;
        saul_reg_add(&(saul_entries[(i * 3) + 1]));
        saul_entries[(i * 3) + 2].dev = &(lsm6dsxx_devs[i]);
        saul_entries[(i * 3) + 2].name = lsm6dsxx_saul_info[i].name;
        saul_entries[(i * 3) + 2].driver = &lsm6dsxx_saul_temp_driver;
        saul_reg_add(&(saul_entries[(i * 3) + 2]));
    }
}
