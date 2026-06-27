/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_vl53l1x
 * @brief       Default configuration for ST VL53L1X Time-of-Flight (ToF) ranging sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include "board.h"
#include "saul_reg.h"

#include "vl53l1x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default hardware configuration parameters
 * @{
 */

/**
 * @brief    Default device is I2C_DEV(0)
 */
#ifndef VL53L1X_PARAM_DEV
#  define VL53L1X_PARAM_DEV             I2C_DEV(0)
#endif

/**
 * @brief    Default I2C slave address
 */
#ifndef VL53L1X_PARAM_ADDR
#  define VL53L1X_PARAM_ADDR            (VL53L1X_I2C_ADDRESS)
#endif

/**
 * @brief   Default I2C VDDIO voltage: 2.8 V
 */
#ifndef VL53L1X_PARAM_VDDIO_2V8
#  define VL53L1X_PARAM_VDDIO_2V8       (true)
#endif

/**
 * @brief   Default interrupt pin: not used
 */
#ifndef VL53L1X_PARAM_PIN_INT
#  define VL53L1X_PARAM_PIN_INT         (GPIO_UNDEF)
#endif

/**
 * @brief   Default shutdown pin: not used
 */
#ifndef VL53L1X_PARAM_PIN_SHUTDOWN
#  define VL53L1X_PARAM_PIN_SHUTDOWN    (GPIO_UNDEF)
#endif
/** @} */

/**
 * @name    Default sensor configuration parameters
 * @{
 */

#if !DOXYGEN

/* Mapping of Kconfig defines to the respective driver enumeration values */
#  ifdef CONFIG_VL53L1X_DIST_SHORT
#    define CONFIG_VL53L1X_PARAM_DISTANCE_MODE      (VL53L1X_DIST_SHORT)
#  elif CONFIG_VL53L1X_DIST_LONG
#    define CONFIG_VL53L1X_PARAM_DISTANCE_MODE      (VL53L1X_DIST_LONG)
#  endif

#  ifdef CONFIG_VL53L1X_PARAM_TIMING_BUDGET_15
#     define CONFIG_VL53L1X_PARAM_TIMING_BUDGET     (15)
#  elif CONFIG_VL53L1X_PARAM_TIMING_BUDGET_20
#     define CONFIG_VL53L1X_PARAM_TIMING_BUDGET     (20)
#  elif    CONFIG_VL53L1X_PARAM_TIMING_BUDGET_33
#     define CONFIG_VL53L1X_PARAM_TIMING_BUDGET     (33)
#  elif    CONFIG_VL53L1X_PARAM_TIMING_BUDGET_50
#     define CONFIG_VL53L1X_PARAM_TIMING_BUDGET     (50)
#  elif    CONFIG_VL53L1X_PARAM_TIMING_BUDGET_100
#     define CONFIG_VL53L1X_PARAM_TIMING_BUDGET     (100)
#  elif    CONFIG_VL53L1X_PARAM_TIMING_BUDGET_200
#     define CONFIG_VL53L1X_PARAM_TIMING_BUDGET     (200)
#  elif    CONFIG_VL53L1X_PARAM_TIMING_BUDGET_500
#     define CONFIG_VL53L1X_PARAM_TIMING_BUDGET     (500)
#  endif

#endif /* !DOXYGEN */

/**
 * @brief   Default inter-measurement period [ms]: 100 ms
 */
#ifndef CONFIG_VL53L1X_PARAM_PERIOD
#  define CONFIG_VL53L1X_PARAM_PERIOD           (100)
#endif

/**
 * @brief   Default Timing budget [ms]: 50 ms
 */
#ifndef CONFIG_VL53L1X_PARAM_TIMING_BUDGET
#  define CONFIG_VL53L1X_PARAM_TIMING_BUDGET    (50)
#endif

/**
 * @brief   Default distance mode: long
 */
#ifndef CONFIG_VL53L1X_PARAM_DISTANCE_MODE
#  define CONFIG_VL53L1X_PARAM_DISTANCE_MODE    (VL53L1X_DIST_LONG)
#endif
/**@}*/

/**
 * @brief   Default VL53L1X configuration parameter set
 */
#ifndef VL53L1X_PARAMS
#  if !IS_USED(MODULE_VL53L1X_BASIC)
#    define VL53L1X_PARAMS { \
                                .i2c_dev = VL53L1X_PARAM_DEV, \
                                .i2c_addr = VL53L1X_PARAM_ADDR, \
                                .vddio_2v8 = VL53L1X_PARAM_VDDIO_2V8, \
                                .pin_int = VL53L1X_PARAM_PIN_INT, \
                                .pin_shutdown = VL53L1X_PARAM_PIN_SHUTDOWN, \
                                .budget = CONFIG_VL53L1X_PARAM_TIMING_BUDGET, \
                                .period = CONFIG_VL53L1X_PARAM_PERIOD, \
                                .mode = CONFIG_VL53L1X_PARAM_DISTANCE_MODE, \
                        }
#  else /* !IS_USED(MODULE_VL53L1X_BASIC) */
#    define VL53L1X_PARAMS  { \
                                .i2c_dev = VL53L1X_PARAM_DEV, \
                                .i2c_addr = VL53L1X_PARAM_ADDR, \
                                .vddio_2v8 = VL53L1X_PARAM_VDDIO_2V8, \
                                .pin_int = VL53L1X_PARAM_PIN_INT, \
                            }
#  endif /* !IS_USED(MODULE_VL53L1X_BASIC) */
#endif /* VL53L1X_PARAMS */

/**
 * @brief   Default SAUL device information
 */
#ifndef VL53L1X_SAUL_INFO
#  define VL53L1X_SAUL_INFO { .name = "vl53l1x" }
#endif

/**
 * @brief   Allocate some memory to store the default configuration
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

/** @} */
