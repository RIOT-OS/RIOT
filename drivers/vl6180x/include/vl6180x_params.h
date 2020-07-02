/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vl6180x
 * @brief       Default configuration for ST VL6180X Ranging and Ambient Light Sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef VL6180X_PARAMS_H
#define VL6180X_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "vl6180x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef VL6180X_PARAM_DEV
/** device is I2C_DEV(0) */
#define VL6180X_PARAM_DEV           I2C_DEV(0)
#endif

#ifndef VL6180X_PARAM_ADDR
/** device address is VL6180X_I2C_ADDR */
#define VL6180X_PARAM_ADDR          (VL6180X_I2C_ADDR)
#endif

#ifndef VL6180X_PARAM_PIN_INT
/** Interrupt pin: not used */
#define VL6180X_PARAM_PIN_INT       (GPIO_UNDEF)
#endif

#ifndef VL6180X_PARAM_PIN_SHUTDOWN
/** Shutdown pin: not used */
#define VL6180X_PARAM_PIN_SHUTDOWN  (GPIO_UNDEF)
#endif

#ifndef VL6180X_PARAM_RNG_PERIOD
/** Ranging inter-measurement period: 100 ms in steps of 10 ms */
#define VL6180X_PARAM_RNG_PERIOD    (10)
#endif

#ifndef VL6180X_PARAM_RNG_MAX_TIME
/** Ranging maximum convergence time: 50 ms */
#define VL6180X_PARAM_RNG_MAX_TIME  (50)
#endif

#ifndef VL6180X_PARAM_ALS_PERIOD
/** ALS inter-measurement period: 500 ms in steps of 10 ms */
#define VL6180X_PARAM_ALS_PERIOD    (50)
#endif

#ifndef VL6180X_PARAM_ALS_INT_TIME
/** ALS integration time: 100 ms */
#define VL6180X_PARAM_ALS_INT_TIME  (100)
#endif

#ifndef VL6180X_PARAM_ALS_GAIN
/** ALS analogue light channel gain: 1.0 */
#define VL6180X_PARAM_ALS_GAIN    (VL6180X_ALS_GAIN_1)
#endif

#ifndef VL6180X_PARAMS
#ifndef MODULE_VL6180X_BASIC
#define VL6180X_PARAMS { \
                            .i2c_dev = VL6180X_PARAM_DEV, \
                            .i2c_addr = VL6180X_PARAM_ADDR, \
                            .pin_int = VL6180X_PARAM_PIN_INT, \
                            .pin_shutdown = VL6180X_PARAM_PIN_SHUTDOWN, \
                            .rng_period = VL6180X_PARAM_RNG_PERIOD, \
                            .rng_max_time = VL6180X_PARAM_RNG_MAX_TIME, \
                            .als_period = VL6180X_PARAM_ALS_PERIOD, \
                            .als_int_time = VL6180X_PARAM_ALS_INT_TIME, \
                            .als_gain = VL6180X_PARAM_ALS_GAIN, \
                        }
#else
#define VL6180X_PARAMS { \
                            .i2c_dev = VL6180X_PARAM_DEV, \
                            .i2c_addr = VL6180X_PARAM_ADDR, \
                            .rng_period = VL6180X_PARAM_RNG_PERIOD, \
                            .rng_max_time = VL6180X_PARAM_RNG_MAX_TIME, \
                            .als_period = VL6180X_PARAM_ALS_PERIOD, \
                            .als_int_time = VL6180X_PARAM_ALS_INT_TIME, \
                            .als_gain = VL6180X_PARAM_ALS_GAIN, \
                        }
#endif /* MODULE_VL6180X_BASIC */
#endif /* VL6180X_PARAMS */

#ifndef VL6180X_SAUL_INFO
#define VL6180X_SAUL_INFO          { .name = "vl6180x" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const vl6180x_params_t vl6180x_params[] =
{
    VL6180X_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t vl6180x_saul_info[] =
{
    VL6180X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* VL6180X_PARAMS_H */
/** @} */
