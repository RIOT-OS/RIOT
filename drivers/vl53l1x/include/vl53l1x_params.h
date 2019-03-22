/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vl53l1x
 * @brief       Default configuration for ST VL53L1X Time-of-Flight (ToF) ranging sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef VL53L1X_PARAMS_H
#define VL53L1X_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "vl53l1x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef VL53L1X_PARAM_DEV
/** device is I2C_DEV(0) */
#define VL53L1X_PARAM_DEV           I2C_DEV(0)
#endif

#ifndef VL53L1X_PARAM_VDDIO_2V8
/** VDDIO voltage: 2.8 V */
#define VL53L1X_PARAM_VDDIO_2V8    (true)
#endif

#ifndef VL53L1X_PARAM_PIN_INT
/** Interrupt pin: not used */
#define VL53L1X_PARAM_PIN_INT       (GPIO_UNDEF)
#endif

#ifndef VL53L1X_PARAM_PIN_SHUTDOWN
/** Shutdown pin: not used */
#define VL53L1X_PARAM_PIN_SHUTDOWN  (GPIO_UNDEF)
#endif

#ifndef VL53L1X_PARAM_BUDGET
/** Timing budget in us: 50 ms */
#define VL53L1X_PARAM_BUDGET        (50000)
#endif

#ifndef VL53L1X_PARAM_PERIOD
/** Inter-measurement period in: 100 ms */
#define VL53L1X_PARAM_PERIOD        (100)
#endif

#ifndef VL53L1X_PARAM_MODE
/** Distance mode: long */
#define VL53L1X_PARAM_MODE          (VL53L1X_DIST_LONG)
#endif

#ifndef VL53L1X_PARAMS
#ifndef MODULE_VL53L1X_BASIC
#define VL53L1X_PARAMS { \
                            .i2c_dev = VL53L1X_PARAM_DEV,  \
                            .vddio_2v8 = VL53L1X_PARAM_VDDIO_2V8, \
                            .pin_int = VL53L1X_PARAM_PIN_INT, \
                            .pin_shutdown = VL53L1X_PARAM_PIN_SHUTDOWN, \
                            .budget = VL53L1X_PARAM_BUDGET, \
                            .period = VL53L1X_PARAM_PERIOD, \
                            .mode = VL53L1X_PARAM_MODE, \
                        }
#else /* MODULE_VL53L1X_BASIC */
#define VL53L1X_PARAMS { \
                            .i2c_dev = VL53L1X_PARAM_DEV,  \
                            .vddio_2v8 = VL53L1X_PARAM_VDDIO_2V8, \
                            .pin_int = VL53L1X_PARAM_PIN_INT, \
                        }
#endif /* MODULE_VL53L1X_BASIC */
#endif /* VL53L1X_PARAMS */

#ifndef VL53L1X_SAUL_INFO
#define VL53L1X_SAUL_INFO          { .name = "vl53l1x" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const vl53l1x_params_t vl53l1x_params[] =
{
    VL53L1X_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t vl53l1x_saul_info[] =
{
    VL53L1X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* VL53L1X_PARAMS_H */
/** @} */
