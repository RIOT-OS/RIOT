/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vl6180x
 * @brief       Default configuration for ST VL6180X Ranging and Ambient Light Sensing (ALS) module
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
 * @name    Default hardware configuration
 * @{
 */
#ifndef VL6180X_PARAM_DEV
/** Default I2C_DEV(0) device */
#define VL6180X_PARAM_DEV               I2C_DEV(0)
#endif

#ifndef VL6180X_PARAM_ADDR
/** Default I2C device address */
#define VL6180X_PARAM_ADDR              (VL6180X_I2C_ADDR)
#endif

#ifndef VL6180X_PARAM_INT_PIN
/** Default interrupt pin */
#define VL6180X_PARAM_INT_PIN           (GPIO_PIN(0, 1))
#endif

#ifndef VL6180X_PARAM_SHUTDOWN_PIN
/** Default shutdown pin */
#define VL6180X_PARAM_SHUTDOWN_PIN      (GPIO_PIN(0, 2))
#endif
/** @} */

/**
 * @name    Default sensor configuration parameters
 * @{
 */
#if !DOXYGEN
/* Mapping of Kconfig defines to the respective driver enumeration values */

#ifdef CONFIG_VL6180X_ALS_GAIN_1
#define CONFIG_VL6180X_ALS_GAIN         (VL6180X_ALS_GAIN_1)
#elif CONFIG_VL6180X_ALS_GAIN_1_25
#define CONFIG_VL6180X_ALS_GAIN         (VL6180X_ALS_GAIN_1_25)
#elif CONFIG_VL6180X_ALS_GAIN_1_67
#define CONFIG_VL6180X_ALS_GAIN         (VL6180X_ALS_GAIN_1_67)
#elif CONFIG_VL6180X_ALS_GAIN_2_5
#define CONFIG_VL6180X_ALS_GAIN         (VL6180X_ALS_GAIN_2_5)
#elif CONFIG_VL6180X_ALS_GAIN_5
#define CONFIG_VL6180X_ALS_GAIN         (VL6180X_ALS_GAIN_5)
#elif CONFIG_VL6180X_ALS_GAIN_10
#define CONFIG_VL6180X_ALS_GAIN         (VL6180X_ALS_GAIN_10)
#elif CONFIG_VL6180X_ALS_GAIN_20
#define CONFIG_VL6180X_ALS_GAIN         (VL6180X_ALS_GAIN_20)
#elif CONFIG_VL6180X_ALS_GAIN_40
#define CONFIG_VL6180X_ALS_GAIN         (VL6180X_ALS_GAIN_40)
#endif

#ifdef CONFIG_VL6180X_RNG_INT_DRDY
#define CONFIG_VL6180X_RNG_INT          (VL6180X_INT_DRDY)
#elif CONFIG_VL6180X_RNG_INT_LOW
#define CONFIG_VL6180X_RNG_INT          (VL6180X_INT_LOW)
#elif CONFIG_VL6180X_RNG_INT_HIGH
#define CONFIG_VL6180X_RNG_INT          (VL6180X_INT_HIGH)
#elif CONFIG_VL6180X_RNG_INT_OUT
#define CONFIG_VL6180X_RNG_INT          (VL6180X_INT_OUT)
#endif

#ifdef CONFIG_VL6180X_ALS_INT_DRDY
#define CONFIG_VL6180X_ALS_INT          (VL6180X_INT_DRDY)
#elif CONFIG_VL6180X_ALS_INT_LOW
#define CONFIG_VL6180X_ALS_INT          (VL6180X_INT_LOW)
#elif CONFIG_VL6180X_ALS_INT_HIGH
#define CONFIG_VL6180X_ALS_INT          (VL6180X_INT_HIGH)
#elif CONFIG_VL6180X_ALS_INT_OUT
#define CONFIG_VL6180X_ALS_INT          (VL6180X_INT_OUT)
#endif

#endif /* !DOXYGEN */

#ifndef CONFIG_VL6180X_MEAS_PERIOD
/** Default period for range and ALS measurements in steps of 10 ms: 200 ms */
#define CONFIG_VL6180X_MEAS_PERIOD      (20)
#endif

#ifndef CONFIG_VL6180X_RNG_MAX_TIME
/** Default ranging maximum convergence time: 50 ms */
#define CONFIG_VL6180X_RNG_MAX_TIME     (50)
#endif

#ifndef CONFIG_VL6180X_RNG_INT
/** Default interrupt mode for ranging: VL6180X_INT_DRDY */
#define CONFIG_VL6180X_RNG_INT          (VL6180X_INT_DRDY)
#endif

#ifndef CONFIG_VL6180X_RNG_THRESH_LOW
/** Default low threshold value for ranging comparison: 20 mm */
#define CONFIG_VL6180X_RNG_THRESH_LOW   (20)
#endif

#ifndef CONFIG_VL6180X_RNG_THRESH_HIGH
/** Default high threshold value for ranging comparison: 90 mm */
#define CONFIG_VL6180X_RNG_THRESH_HIGH  (90)
#endif

#ifndef CONFIG_VL6180X_ALS_INT_TIME
/** Default ALS integration time: 100 ms (recommended by the datasheet) */
#define CONFIG_VL6180X_ALS_INT_TIME     (100)
#endif

#ifndef CONFIG_VL6180X_ALS_GAIN
/** Default ALS analogue light channel gain: 1.0 */
#define CONFIG_VL6180X_ALS_GAIN         (VL6180X_ALS_GAIN_1)
#endif

#ifndef CONFIG_VL6180X_ALS_LUX_RES
/** Default ALS lux resolution specified as lux/count*1000: 0.32 count/lux is factory calibrated */
#define CONFIG_VL6180X_ALS_LUX_RES      320
#endif

#ifndef CONFIG_VL6180X_ALS_INT
/** Default interrupt mode for ranging: VL6180X_INT_DRDY */
#define CONFIG_VL6180X_ALS_INT          (VL6180X_INT_DRDY)
#endif

#ifndef CONFIG_VL6180X_ALS_THRESH_LOW
/** Default low threshold value for ALS comparison: 50 counts */
#define CONFIG_VL6180X_ALS_THRESH_LOW   (50)
#endif

#ifndef CONFIG_VL6180X_ALS_THRESH_HIGH
/** Default high threshold value for ALS comparison: 2000 counts */
#define CONFIG_VL6180X_ALS_THRESH_HIGH  (2000)
#endif

#if IS_USED(MODULE_VL6180X_RNG) || DOXYGEN
/** Range measurement configuration parameters */
#define VL6180X_PARAM_RANGE         .rng_max_time = CONFIG_VL6180X_RNG_MAX_TIME,
#else
#define VL6180X_PARAM_RANGE
#endif

#if IS_USED(MODULE_VL6180X_ALS) || DOXYGEN
/** ALS measurement configuration parameters */
#define VL6180X_PARAM_ALS           .als_int_time = CONFIG_VL6180X_ALS_INT_TIME, \
                                    .als_gain = CONFIG_VL6180X_ALS_GAIN, \
                                    .als_lux_res = CONFIG_VL6180X_ALS_LUX_RES,
#else
#define VL6180X_PARAM_ALS
#endif

#if IS_USED(MODULE_VL6180X_SHUTDOWN) || DOXYGEN
/** Shutdown hardware configuration */
#define VL6180X_PARAM_SHUTDOWN      .shutdown_pin = VL6180X_PARAM_SHUTDOWN_PIN,
#else
#define VL6180X_PARAM_SHUTDOWN
#endif

#if IS_USED(MODULE_VL6180X_IRQ) || DOXYGEN
/** Interrupt pin configuration */
#define VL6180X_PARAM_INT           .int_pin = VL6180X_PARAM_INT_PIN,

#if IS_USED(MODULE_VL6180X_RNG) || DOXYGEN
/** Interrupt configuration for ranging */
#define VL6180X_PARAM_INT_RNG_CFG   .int_cfg.rng_int = CONFIG_VL6180X_RNG_INT, \
                                    .int_thresh.rng_low = CONFIG_VL6180X_RNG_THRESH_LOW, \
                                    .int_thresh.rng_high = CONFIG_VL6180X_RNG_THRESH_HIGH,
#else /* IS_USED(MODULE_VL6180X_RNG) || DOXYGEN */
#define VL6180X_PARAM_INT_RNG_CFG
#endif /* IS_USED(MODULE_VL6180X_RNG) || DOXYGEN */

#if IS_USED(MODULE_VL6180X_ALS) || DOXYGEN
/** Interrupt configuration for ALS */
#define VL6180X_PARAM_INT_ALS_CFG   .int_cfg.als_int = CONFIG_VL6180X_ALS_INT, \
                                    .int_thresh.als_low = CONFIG_VL6180X_ALS_THRESH_LOW, \
                                    .int_thresh.als_high = CONFIG_VL6180X_ALS_THRESH_HIGH,
#else /* IS_USED(MODULE_VL6180X_ALS) || DOXYGEN */
#define VL6180X_PARAM_INT_ALS_CFG
#endif /* IS_USED(MODULE_VL6180X_ALS) || DOXYGEN */

#else /* IS_USED(MODULE_VL6180X_IRQ) || DOXYGEN */
#define VL6180X_PARAM_INT
#define VL6180X_PARAM_INT_RNG_CFG
#define VL6180X_PARAM_INT_ALS_CFG
#endif /* IS_USED(MODULE_VL6180X_IRQ) || DOXYGEN */

#if !VL6180X_PARAMS || DOXYGEN
/** Default configuration parameter set */
#define VL6180X_PARAMS { \
                            .i2c_dev = VL6180X_PARAM_DEV, \
                            .i2c_addr = VL6180X_PARAM_ADDR, \
                            .period = CONFIG_VL6180X_MEAS_PERIOD, \
                            VL6180X_PARAM_RANGE \
                            VL6180X_PARAM_ALS \
                            VL6180X_PARAM_SHUTDOWN \
                            VL6180X_PARAM_INT \
                            VL6180X_PARAM_INT_RNG_CFG \
                            VL6180X_PARAM_INT_ALS_CFG \
                        }
#endif /* !VL6180X_PARAMS */

#if !defined(VL6180X_SAUL_INFO) || DOXYGEN
/** Default SAUL information */
#define VL6180X_SAUL_INFO   { .name = "vl6180x" }
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
