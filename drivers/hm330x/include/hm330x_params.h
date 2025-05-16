/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_hm330x
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */


#include "board.h"
#include "saul_reg.h"

#include "hm330x.h"
#include "hm330x_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
/**
 * @brief   HM330X default I2C bus
 */
#ifndef HM330X_PARAM_I2C_DEV
#define HM330X_PARAM_I2C_DEV       I2C_DEV(0)
#endif
/**
 * @brief   HM330X default reset pin
 */
#ifndef HM330X_PARAM_RESET_PIN
#define HM330X_PARAM_RESET_PIN     GPIO_UNDEF
#endif
/**
 * @brief   HM330X default set pin
 */
#ifndef HM330X_PARAM_SET_PIN
#define HM330X_PARAM_SET_PIN       GPIO_UNDEF
#endif
/**
 * @brief   HM330X default SAUL information
 */
#ifndef HM330X_SAUL_INFO
#define HM330X_SAUL_INFO           { .name = "hm330x" }
#endif
/**
 * @brief   HM330X default parameters
 */
#ifndef HM330X_PARAMS
#define HM330X_PARAMS              { .i2c = HM330X_PARAM_I2C_DEV, \
                                     .reset_pin = HM330X_PARAM_RESET_PIN, \
                                     .set_pin = HM330X_PARAM_SET_PIN }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const hm330x_params_t hm330x_params[] =
{
    HM330X_PARAMS
};

/**
 * @brief   Define the number of configured sensors
 */
#define HM330X_NUMOF     ARRAY_SIZE(hm330x_params)

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t hm330x_saul_info[] =
{
    HM330X_SAUL_INFO
};

/**
 * @brief   Number of saul info structs
 */
#define HM330X_INFO_NUM ARRAY_SIZE(hm330x_saul_info)


#ifdef __cplusplus
}
#endif

/** @} */
