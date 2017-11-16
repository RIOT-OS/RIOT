/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_iotlab-m3
 * @{
 *
 * @file
 * @brief     LSM303DLHC board specific configuration
 *
 * @author    Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LSM303DLHC_PARAMS_H
#define LSM303DLHC_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LSM303DLHC configuration
 */
static const  lsm303dlhc_params_t lsm303dlhc_params[] =
{
    {
        .i2c = LSM303DLHC_I2C,
        .acc_addr = LSM303DLHC_ACC_ADDR,
        .acc_pin = LSM303DLHC_INT1,
        .acc_rate = LSM303DLHC_ACC_SAMPLE_RATE_10HZ,
        .acc_scale = LSM303DLHC_ACC_SCALE_4G,
        .mag_addr = LSM303DLHC_MAG_ADDR,
        .mag_pin = LSM303DLHC_DRDY,
        .mag_rate = LSM303DLHC_MAG_SAMPLE_RATE_15HZ,
        .mag_gain = LSM303DLHC_MAG_GAIN_450_400_GAUSS,
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lsm303dlhc_saul_info[] =
{
    {
        .name = "lsm303dlhc",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* LSM303DLHC_PARAMS_H */
/** @} */
