/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tmp006
 *
 * @{
 * @file
 * @brief       Default configuration for TMP006 devices
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef TMP006_PARAMS_H
#define TMP006_PARAMS_H

#include "board.h"
#include "tmp006.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the TMP006 driver
 * @{
 */
#ifndef TMP006_PARAM_I2C
#define TMP006_PARAM_I2C           I2C_DEV(0)
#endif
#ifndef TMP006_PARAM_ADDR
#define TMP006_PARAM_ADDR          (TMP006_I2C_ADDRESS)
#endif
#ifndef TMP006_PARAM_RATE
#define TMP006_PARAM_RATE           TMP006_CONFIG_CR_DEF
#endif

#ifndef TMP006_PARAMS
#define TMP006_PARAMS              { .i2c  = TMP006_PARAM_I2C,  \
                                     .addr = TMP006_PARAM_ADDR, \
                                     .rate = TMP006_PARAM_RATE }
#endif
#ifndef TMP006_SAUL_INFO
#define TMP006_SAUL_INFO           { .name = "tmp006" }
#endif
/**@}*/

/**
 * @brief   HDC1000 configuration
 */
static const tmp006_params_t tmp006_params[] =
{
    TMP006_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t tmp006_saul_info[] =
{
    TMP006_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* TMP006_PARAMS_H */
/** @} */
