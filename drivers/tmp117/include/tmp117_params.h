/*
 * SPDX-FileCopyrightText: 2026 UGA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_tmp117
 * @ingroup     drivers_sensors
 * @brief       Driver for TMP117 temperature sensor
 *
 * @{
 * @file
 * @brief		Parameters definition for the TMP117 temperature sensor
 * @author      léo cordier <leo.cordier@univ-grenoble-alpes.fr>
 */

#include "board.h"
#include "tmp117.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default configuration parameters for TMP117 sensors
 * @{
 */
#ifndef TMP117_PARAM_I2C
#  define TMP117_PARAM_I2C      (I2C_DEV(0))                /**< default TMP117 i2c peripheral */
#endif
#ifndef TMP117_PARAM_ADDR
#  define TMP117_PARAM_ADDR     (0x48)                      /**< default TMP117 i2c address */
#endif
#ifndef TMP117_PARAM_CONV_MODE
#  define TMP117_PARAM_CONV_MODE    TMP117_CONV_CC          /**< default TMP117 conversion mode */
#endif
#ifndef TMP117_PARAM_CONV_CYCLE
#  define TMP117_PARAM_CONV_CYCLE   TMP117_CONV_CYCLE_1_S   /**< default TMP117 conversion cycle */
#endif
#ifndef TMP117_PARAM_AVG
#  define TMP117_PARAM_AVG          TMP117_AVG_8            /**< default TMP117 averaging */
#endif

#ifndef TMP117_PARAMS
#  define TMP117_PARAMS       { .i2c = TMP117_PARAM_I2C, \
                                .addr = TMP117_PARAM_ADDR, \
                                .conv_mode = TMP117_PARAM_CONV_MODE, \
                                .conv_cycle = TMP117_PARAM_CONV_CYCLE, \
                                .avg = TMP117_PARAM_AVG }   /**< default TMP117 parameters */
#endif
/** @} */

#ifndef TMP117_SAUL_INFO
/**
 * @brief   The SAUL info to register the TMP117 device driver instances with
 */
#define TMP117_SAUL_INFO           { .name = "tmp117" }
#endif
/**@}*/

/**
 * @brief   Allocation of TMP117 configuration
 */
static const tmp117_params_t tmp117_params[] =
{
    TMP117_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t tmp117_saul_info[] =
{
    TMP117_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
