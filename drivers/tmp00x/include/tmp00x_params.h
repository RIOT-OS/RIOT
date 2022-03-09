/*
 * Copyright (C) 2017 - 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tmp00x
 *
 * @{
 * @file
 * @brief       Default configuration for TMP00X (TMP006 and TMP007) devices
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef TMP00X_PARAMS_H
#define TMP00X_PARAMS_H

#include "board.h"
#include "tmp00x.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the TMP00X driver
 * @ingroup  config
 * @{
 */
#ifndef TMP00X_PARAM_I2C
#define TMP00X_PARAM_I2C           I2C_DEV(0)
#endif
#ifndef TMP00X_PARAM_ADDR
#define TMP00X_PARAM_ADDR          (CONFIG_TMP00X_I2C_ADDRESS)
#endif
#ifndef TMP00X_PARAM_RATE
#define TMP00X_PARAM_RATE           TMP00X_CONFIG_CR_DEF
#endif

#ifndef TMP00X_PARAMS
#define TMP00X_PARAMS              { .i2c  = TMP00X_PARAM_I2C,  \
                                     .addr = TMP00X_PARAM_ADDR, \
                                     .rate = TMP00X_PARAM_RATE }
#endif
#ifndef TMP00X_SAUL_INFO
#define TMP00X_SAUL_INFO           { .name = "tmp00x" }
#endif
/**@}*/

/**
 * @brief   TMP00X configuration
 */
static const tmp00x_params_t tmp00x_params[] =
{
    TMP00X_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t tmp00x_saul_info[] =
{
    TMP00X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* TMP00X_PARAMS_H */
/** @} */
