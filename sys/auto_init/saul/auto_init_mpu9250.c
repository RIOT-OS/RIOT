/*
 * Copyright (C) 2017 Inria
 * Copyright 2018 Ville Hiltunen
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
 * @brief       Auto initialization of MPU9250 accelerometer/magnetometer
 *
 * @note        Original for mpu9150, renamed for mpu9250
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>,
 *              modified by Ville Hiltunen <hiltunenvillej@gmail.com>
 *
 * @}
 */

#ifdef MODULE_MPU9250

#include "log.h"
#include "saul_reg.h"
#include "mpu9250.h"
#include "mpu9250_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MPU9250_NUM         (sizeof(mpu9250_params)/sizeof(mpu9250_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static mpu9250_t mpu9250_devs[MPU9250_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MPU9250_NUM * 3];

/**
 * @brief   Define the number of saul info
 */
#define MPU9250_INFO_NUM    (sizeof(mpu9250_saul_info)/sizeof(mpu9250_saul_info[0]))

/**
 * @brief   Reference the driver structs
 * @{
 */
extern saul_driver_t mpu9250_saul_acc_driver;
extern saul_driver_t mpu9250_saul_gyro_driver;
extern saul_driver_t mpu9250_saul_mag_driver;

/** @} */

void auto_init_mpu9250(void)
{
    assert(MPU9250_NUM == MPU9250_INFO_NUM);

    for (unsigned int i = 0; i < MPU9250_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mpu9250 #%u\n", i);

        if (mpu9250_init(&mpu9250_devs[i], &mpu9250_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing mpu9250 #%u\n", i);
            continue;
        }

        saul_entries[(i * 3)].dev = &(mpu9250_devs[i]);
        saul_entries[(i * 3)].name = mpu9250_saul_info[i].name;
        saul_entries[(i * 3)].driver = &mpu9250_saul_acc_driver;
        saul_entries[(i * 3) + 1].dev = &(mpu9250_devs[i]);
        saul_entries[(i * 3) + 1].name = mpu9250_saul_info[i].name;
        saul_entries[(i * 3) + 1].driver = &mpu9250_saul_gyro_driver;
        saul_entries[(i * 3) + 2].dev = &(mpu9250_devs[i]);
        saul_entries[(i * 3) + 2].name = mpu9250_saul_info[i].name;
        saul_entries[(i * 3) + 2].driver = &mpu9250_saul_mag_driver;
        saul_reg_add(&(saul_entries[(i * 3)]));
        saul_reg_add(&(saul_entries[(i * 3) + 1]));
        saul_reg_add(&(saul_entries[(i * 3) + 2]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_MPU9250 */
