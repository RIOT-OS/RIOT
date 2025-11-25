/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lsm303dlhc
 *
 * @{
 * @file
 * @brief       Default configuration for LSM303DLHC devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LSM303DLHC_PARAMS_H
#define LSM303DLHC_PARAMS_H

#include "board.h"
#include "lsm303dlhc.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters
 * @{
 */
#ifndef LSM303DLHC_PARAM_I2C
#define LSM303DLHC_PARAM_I2C            I2C_DEV(0)
#endif
#ifndef LSM303DLHC_PARAM_ACC_ADDR
#define LSM303DLHC_PARAM_ACC_ADDR       (0x19)
#endif
#ifndef LSM303DLHC_PARAM_ACC_PIN
#define LSM303DLHC_PARAM_ACC_PIN        (GPIO_PIN(0, 0))
#endif
#ifndef LSM303DLHC_PARAM_ACC_RATE
#define LSM303DLHC_PARAM_ACC_RATE       (LSM303DLHC_ACC_SAMPLE_RATE_10HZ)
#endif
#ifndef LSM303DLHC_PARAM_ACC_SCALE
#define LSM303DLHC_PARAM_ACC_SCALE      (LSM303DLHC_ACC_SCALE_4G)
#endif
#ifndef LSM303DLHC_PARAM_MAG_ADDR
#define LSM303DLHC_PARAM_MAG_ADDR       (0x1e)
#endif
#ifndef LSM303DLHC_PARAM_MAG_PIN
#define LSM303DLHC_PARAM_MAG_PIN        (GPIO_PIN(0, 1))
#endif
#ifndef LSM303DLHC_PARAM_MAG_RATE
#define LSM303DLHC_PARAM_MAG_RATE       (LSM303DLHC_MAG_SAMPLE_RATE_15HZ)
#endif
#ifndef LSM303DLHC_PARAM_MAG_GAIN
#define LSM303DLHC_PARAM_MAG_GAIN       (LSM303DLHC_MAG_GAIN_450_400_GAUSS)
#endif

#ifndef LSM303DLHC_PARAMS
#define LSM303DLHC_PARAMS               { .i2c       = LSM303DLHC_PARAM_I2C,       \
                                          .acc_addr  = LSM303DLHC_PARAM_ACC_ADDR,  \
                                          .acc_pin   = LSM303DLHC_PARAM_ACC_PIN,   \
                                          .acc_rate  = LSM303DLHC_PARAM_ACC_RATE,  \
                                          .acc_scale = LSM303DLHC_PARAM_ACC_SCALE, \
                                          .mag_addr  = LSM303DLHC_PARAM_MAG_ADDR,  \
                                          .mag_pin   = LSM303DLHC_PARAM_MAG_PIN,   \
                                          .mag_rate  = LSM303DLHC_PARAM_MAG_RATE,  \
                                          .mag_gain  = LSM303DLHC_PARAM_MAG_GAIN }
#endif
#ifndef LSM303DLHC_SAUL_INFO
#define LSM303DLHC_SAUL_INFO            { .name = "lsm303dlhc" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const lsm303dlhc_params_t lsm303dlhc_params[] =
{
    LSM303DLHC_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lsm303dlhc_saul_info[] =
{
    LSM303DLHC_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* LSM303DLHC_PARAMS_H */
/** @} */
