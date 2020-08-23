/*
 * Copyright (C) 2020 Viktor Gal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hdc2010
 *
 * @{
 * @file
 * @brief       Default configuration for HDC2010 devices
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 */

#ifndef HDC2010_PARAMS_H
#define HDC2010_PARAMS_H

#include "board.h"
#include "hdc2010.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the HDC2010 driver
 * @{
 */
#ifndef HDC2010_PARAM_I2C
#define HDC2010_PARAM_I2C            I2C_DEV(0)
#endif
#ifndef HDC2010_PARAM_ADDR
#define HDC2010_PARAM_ADDR           (CONFIG_HDC2010_I2C_ADDRESS)
#endif
#ifndef HDC2010_PARAM_RES
#define HDC2010_PARAM_RES            HDC2010_14BIT
#endif
#ifndef HDC2010_PARAM_RENEW_INTERVAL
#define HDC2010_PARAM_RENEW_INTERVAL (1000000ul)
#endif
#ifndef HDC2010_PARAM_MODE
#define HDC2010_PARAM_MODE           HDC2010_MEAS_HUM_AND_TEMP
#endif
#ifndef HDC2010_PARAM_AMM
#define HDC2010_PARAM_AMM            HDC2010_TRIGGER
#endif

#ifndef HDC2010_PARAMS
#define HDC2010_PARAMS               { .i2c  = HDC2010_PARAM_I2C,  \
                                       .addr = HDC2010_PARAM_ADDR, \
                                       .res  = HDC2010_PARAM_RES, \
                                       .mode = HDC2010_PARAM_MODE, \
                                       .amm  = HDC2010_PARAM_AMM, \
                                       .renew_interval = HDC2010_PARAM_RENEW_INTERVAL }
#endif
#ifndef HDC2010_SAUL_INFO
#define HDC2010_SAUL_INFO            { .name = "hdc2010" }
#endif
/**@}*/

/**
 * @brief   HDC2010 configuration
 */
static const hdc2010_params_t hdc2010_params[] =
{
    HDC2010_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t hdc2010_saul_info[] =
{
    HDC2010_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* HDC2010_PARAMS_H */
/** @} */
