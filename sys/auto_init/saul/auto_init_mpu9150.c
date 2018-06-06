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
 * @brief       Auto initialization of MPU9150 accelerometer/magnetometer
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifdef MODULE_MPU9150

#include "log.h"
#include "saul_reg.h"
#include "mpu9150.h"
#include "mpu9150_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MPU9150_NUM         (sizeof(mpu9150_params) / sizeof(mpu9150_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static mpu9150_t mpu9150_devs[MPU9150_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MPU9150_NUM * 3];

/**
 * @brief   Define the number of saul info
 */
#define MPU9150_INFO_NUM    (sizeof(mpu9150_saul_info) / sizeof(mpu9150_saul_info[0]))

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t mpu9150_saul_acc_driver;
extern saul_driver_t mpu9150_saul_gyro_driver;
extern saul_driver_t mpu9150_saul_mag_driver;

/** @} */

void auto_init_mpu9150(void)
{
    assert(MPU9150_NUM == MPU9150_INFO_NUM);

    for (unsigned int i = 0; i < MPU9150_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mpu9150 #%u\n", i);

        if (mpu9150_init(&mpu9150_devs[i], &mpu9150_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing mpu9150 #%u\n", i);
            continue;
        }

        saul_entries[(i * 3)].dev = &(mpu9150_devs[i]);
        saul_entries[(i * 3)].name = mpu9150_saul_info[i].name;
        saul_entries[(i * 3)].driver = &mpu9150_saul_acc_driver;
        saul_entries[(i * 3) + 1].dev = &(mpu9150_devs[i]);
        saul_entries[(i * 3) + 1].name = mpu9150_saul_info[i].name;
        saul_entries[(i * 3) + 1].driver = &mpu9150_saul_gyro_driver;
        saul_entries[(i * 3) + 2].dev = &(mpu9150_devs[i]);
        saul_entries[(i * 3) + 2].name = mpu9150_saul_info[i].name;
        saul_entries[(i * 3) + 2].driver = &mpu9150_saul_mag_driver;
        saul_reg_add(&(saul_entries[(i * 3)]));
        saul_reg_add(&(saul_entries[(i * 3) + 1]));
        saul_reg_add(&(saul_entries[(i * 3) + 2]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_MPU9150 */
