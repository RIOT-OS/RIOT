/*
 * Copyright (C) 2019 Wouter Horlings
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bno055
 * @{
 *
 * @file
 * @brief       Device driver interface for the BNO055 9-axis sensor
 *
 * @author      Wouter Horlings <wouter@horlin.gs>
 * @}
 */

#ifndef BNO055_PARAMS_H
#define BNO055_PARAMS_H

#include "bno055_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

/**
 * @name   Default configuration parameters for device BNO055
 * @{
 */
#ifndef BNO055_PARAM_I2C
#define BNO055_PARAM_I2C        I2C_DEV(0)
#endif
#ifndef BNO055_PARAM_ADDR
#define BNO055_PARAM_ADDR       BNO055_ADDR_DEFAULT
#endif
#ifndef BNO055_PARAM_INT1
#define BNO055_PARAM_INT1       GPIO_PIN(0, 0)
#endif
#ifndef BNO055_PARAM_INT2
#define BNO055_PARAM_INT2       GPIO_PIN(0, 1)
#endif
#ifndef BNO055_PARAM_MAG_RATE
#define BNO055_PARAM_MAG_RATE   BNO055_MAG_DRATE_DEFAULT
#endif
#ifndef BNO055_PARAM_ACC_RANGE
#define BNO055_PARAM_ACC_RANGE  BNO055_ACC_RANGE_2G
#endif
#ifndef BNO055_PARAM_GYRO_SCALE
#define BNO055_PARAM_GYRO_SCALE BNO055_GYRO_SCALE_2000DPS
#endif
#ifndef BNO055_PARAM_OPR_MODE
#define BNO055_PARAM_OPR_MODE BNO055_OPR_MODE_NDOF
#endif
#ifndef BNO055_PARAM_PWR_MODE
#define BNO055_PARAM_PWR_MODE BNO055_PWR_MODE_NORMAL
#endif
#ifndef BNO055_PARAM_EUL_UNIT
#define BNO055_PARAM_EUL_UNIT BNO055_BIT_EUL_RAD
#endif
#ifndef BNO055_PARAM_TEMP_UNIT
#define BNO055_PARAM_TEMP_UNIT BNO055_BIT_TEMP_DC
#endif
#ifndef BNO055_PARAM_ACC_UNIT
#define BNO055_PARAM_ACC_UNIT BNO055_BIT_ACC_MSS
#endif
#ifndef BNO055_PARAM_GYR_UNIT
#define BNO055_PARAM_GYR_UNIT BNO055_BIT_GYR_RPS
#endif
#ifndef BNO055_PARAMS
#define BNO055_PARAMS                         \
    {   .i2c = BNO055_PARAM_I2C,              \
        .addr = BNO055_PARAM_ADDR,            \
        .opr = BNO055_PARAM_OPR_MODE,         \
        .pwr = BNO055_PARAM_PWR_MODE,         \
        .unit_sel = BNO055_PARAM_TEMP_UNIT |  \
                    BNO055_PARAM_EUL_UNIT |   \
                    BNO055_PARAM_GYR_UNIT |   \
                    BNO055_PARAM_ACC_UNIT,    \
        .gpio1 = BNO055_PARAM_INT1,           \
        .gpio2 = BNO055_PARAM_INT2,           \
        .mag_rate = BNO055_PARAM_MAG_RATE,    \
        .acc_range = BNO055_PARAM_ACC_RANGE,  \
        .acc_unit = BNO055_PARAM_ACC_UNIT,    \
        .gyro_scale = BNO055_PARAM_GYRO_SCALE \
    }
#endif

/** @} */

/**
 * @brief   BNO055 configuration
 */
static const bno055_params_t bno055_params[] = {
    BNO055_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* BNO055_PARAMS_H */
/** @} */
