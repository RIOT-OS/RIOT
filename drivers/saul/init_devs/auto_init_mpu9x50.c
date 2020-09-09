/*
 * Copyright (C) 2017 Inria
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
 * @brief       Auto initialization of MPU9X50 (MPU9150 and MPU9250) accelerometer/magnetometer
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "mpu9x50.h"
#include "mpu9x50_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MPU9X50_NUM         ARRAY_SIZE(mpu9x50_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static mpu9x50_t mpu9x50_devs[MPU9X50_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MPU9X50_NUM * 3];

/**
 * @brief   Define the number of saul info
 */
#define MPU9X50_INFO_NUM    ARRAY_SIZE(mpu9x50_saul_info)

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t mpu9x50_saul_acc_driver;
extern saul_driver_t mpu9x50_saul_gyro_driver;
extern saul_driver_t mpu9x50_saul_mag_driver;

/** @} */

void auto_init_mpu9x50(void)
{
    assert(MPU9X50_NUM == MPU9X50_INFO_NUM);

    for (unsigned int i = 0; i < MPU9X50_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mpu9x50 #%u\n", i);

        if (mpu9x50_init(&mpu9x50_devs[i], &mpu9x50_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing mpu9x50 #%u\n", i);
            continue;
        }

        saul_entries[(i * 3)].dev = &(mpu9x50_devs[i]);
        saul_entries[(i * 3)].name = mpu9x50_saul_info[i].name;
        saul_entries[(i * 3)].driver = &mpu9x50_saul_acc_driver;
        saul_entries[(i * 3) + 1].dev = &(mpu9x50_devs[i]);
        saul_entries[(i * 3) + 1].name = mpu9x50_saul_info[i].name;
        saul_entries[(i * 3) + 1].driver = &mpu9x50_saul_gyro_driver;
        saul_entries[(i * 3) + 2].dev = &(mpu9x50_devs[i]);
        saul_entries[(i * 3) + 2].name = mpu9x50_saul_info[i].name;
        saul_entries[(i * 3) + 2].driver = &mpu9x50_saul_mag_driver;
        saul_reg_add(&(saul_entries[(i * 3)]));
        saul_reg_add(&(saul_entries[(i * 3) + 1]));
        saul_reg_add(&(saul_entries[(i * 3) + 2]));
    }
}
