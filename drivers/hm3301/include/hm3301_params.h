/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hm3301
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef HM3301_PARAMS_H
#define HM3301_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#include "hm3301.h"
#include "hm3301_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef HM3301_PARAM_I2C_DEV
#define HM3301_PARAM_I2C_DEV       I2C_DEV(0)
#endif
#ifndef HM3301_PARAM_RESET_PIN_
#define HM3301_PARAM_RESET_PIN     GPIO_UNDEF
#endif
#ifndef HM3301_PARAM_SET_PIN
#define HM3301_PARAM_SET_PIN       GPIO_UNDEF
#endif
#ifndef HM3301_SAUL_INFO
#define HM3301_SAUL_INFO           { .name = "hm3301" }
#endif
#ifndef HM3301_PARAMS
#define HM3301_PARAMS              { .i2c = HM3301_PARAM_I2C_DEV, \
                                     .reset_pin = HM3301_PARAM_RESET_PIN, \
                                     .set_pin = HM3301_PARAM_SET_PIN }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const hm3301_params_t hm3301_params[] =
{
    HM3301_PARAMS
};

/**
 * @brief   Define the number of configured sensors
 */
#define HM3301_NUMOF     ARRAY_SIZE(hm3301_params)

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t hm3301_saul_info[] =
{
    HM3301_SAUL_INFO
};

/**
 * @brief   Number of saul info structs
 */
#define HM3301_INFO_NUM ARRAY_SIZE(hm3301_saul_info)


#ifdef __cplusplus
}
#endif

#endif /* HM3301_PARAMS_H */
/** @} */
