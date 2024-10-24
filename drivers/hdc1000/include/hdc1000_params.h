/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hdc1000
 *
 * @{
 * @file
 * @brief       Default configuration for HDC1000 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef HDC1000_PARAMS_H
#define HDC1000_PARAMS_H

#include "board.h"
#include "hdc1000.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for the HDC1000 driver
 * @{
 */
#ifndef HDC1000_PARAM_I2C
/**
 * @brief   I2C bus the HDC1000 is connected to
 */
#define HDC1000_PARAM_I2C            I2C_DEV(0)
#endif

#ifndef HDC1000_PARAM_ADDR
/**
 * @brief   I2C address of the HDC1000
 */
#define HDC1000_PARAM_ADDR           (CONFIG_HDC1000_I2C_ADDRESS)
#endif

#ifndef HDC1000_PARAM_RES
/**
 * @brief   Resolution to sample the humidity and temperature at
 */
#define HDC1000_PARAM_RES            HDC1000_14BIT
#endif

#ifndef HDC1000_PARAM_RENEW_INTERVAL
/**
 * @brief   Interval at which the driver should renew the sensor data cache
 */
#define HDC1000_PARAM_RENEW_INTERVAL (1000000ul)
#endif

#ifndef HDC1000_PARAMS
/**
 * @brief   Parameters to initialize the HDC1000 driver with
 */
#define HDC1000_PARAMS               { .i2c  = HDC1000_PARAM_I2C,  \
                                       .addr = HDC1000_PARAM_ADDR, \
                                       .res  = HDC1000_PARAM_RES, \
                                       .renew_interval = HDC1000_PARAM_RENEW_INTERVAL }
#endif

#ifndef HDC1000_SAUL_INFO
/**
 * @brief   SAUL info to register HDC1000 driver instances with
 */
#define HDC1000_SAUL_INFO            { .name = "hdc1000" }
#endif
/**@}*/

/**
 * @brief   HDC1000 configuration
 */
static const hdc1000_params_t hdc1000_params[] =
{
    HDC1000_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t hdc1000_saul_info[] =
{
    HDC1000_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HDC1000_PARAMS_H */
