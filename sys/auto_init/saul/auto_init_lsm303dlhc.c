/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of LSM303DLHC accelerometer/magnetometer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifdef MODULE_LSM303DLHC

#include "saul_reg.h"
#include "lsm303dlhc.h"
#include "lsm303dlhc_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LSM303DLHC_NUM    (sizeof(lsm303dlhc_params)/sizeof(lsm303dlhc_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static lsm303dlhc_t lsm303dlhc_devs[LSM303DLHC_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LSM303DLHC_NUM * 2];

/**
 * @brief   Reference the driver structs
 * @{
 */
extern saul_driver_t lsm303dlhc_saul_acc_driver;
extern saul_driver_t lsm303dlhc_saul_mag_driver;
/** @} */

void auto_init_lsm303dlhc(void)
{
    for (int i = 0; i < LSM303DLHC_NUM; i++) {
        const lsm303dlhc_params_t *p = &lsm303dlhc_params[i];

        DEBUG("[auto_init_saul] initializing lsm303dlhc acc/mag sensor\n");
        int res = lsm303dlhc_init(&lsm303dlhc_devs[i], p->i2c,
                                  p->acc_pin, p->mag_pin,
                                  p->acc_addr, p->acc_rate, p->acc_scale,
                                  p->mag_addr, p->mag_rate, p->mag_gain);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during initialization\n");
        }
        else {
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
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_LSM303DLHC */
