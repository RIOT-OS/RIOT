/*
 * Copyright (C) 2017 OTA keys S.A.
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
 * @brief       Auto initialization of LSM6DSL accelerometer/gyroscope sensors
 *
 * @author      Vincent Dupont <vincent@otakeys.com
 *
 * @}
 */

#ifdef MODULE_LSM6DSL

#include "log.h"
#include "saul_reg.h"
#include "lsm6dsl.h"
#include "lsm6dsl_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LSM6DSL_NUM    (sizeof(lsm6dsl_params) / sizeof(lsm6dsl_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static lsm6dsl_t lsm6dsl_devs[LSM6DSL_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LSM6DSL_NUM * 3];

/**
 * @brief   Define the number of saul info
 */
#define LSM6DSL_INFO_NUM    (sizeof(lsm6dsl_saul_info) / sizeof(lsm6dsl_saul_info[0]))

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t lsm6dsl_saul_acc_driver;
extern saul_driver_t lsm6dsl_saul_gyro_driver;
extern saul_driver_t lsm6dsl_saul_temp_driver;
/** @} */


void auto_init_lsm6dsl(void)
{
    assert(LSM6DSL_NUM == LSM6DSL_INFO_NUM);

    for (unsigned int i = 0; i < LSM6DSL_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing lsm6dsl #%u\n", i);

        if (lsm6dsl_init(&lsm6dsl_devs[i], &lsm6dsl_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing lsm6dsl #%u\n", i);
            continue;
        }

        saul_entries[(i * 3)].dev = &(lsm6dsl_devs[i]);
        saul_entries[(i * 3)].name = lsm6dsl_saul_info[i].name;
        saul_entries[(i * 3)].driver = &lsm6dsl_saul_acc_driver;
        saul_reg_add(&(saul_entries[(i * 3)]));
        saul_entries[(i * 3) + 1].dev = &(lsm6dsl_devs[i]);
        saul_entries[(i * 3) + 1].name = lsm6dsl_saul_info[i].name;
        saul_entries[(i * 3) + 1].driver = &lsm6dsl_saul_gyro_driver;
        saul_reg_add(&(saul_entries[(i * 3) + 1]));
        saul_entries[(i * 3) + 2].dev = &(lsm6dsl_devs[i]);
        saul_entries[(i * 3) + 2].name = lsm6dsl_saul_info[i].name;
        saul_entries[(i * 3) + 2].driver = &lsm6dsl_saul_temp_driver;
        saul_reg_add(&(saul_entries[(i * 3) + 2]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_LSM6DSL */
