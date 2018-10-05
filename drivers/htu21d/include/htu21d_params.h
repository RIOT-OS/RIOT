/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_htu21d
 * @brief       Default configuration for HTU21D humidity and temperature sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef HTU21D_PARAMS_H
#define HTU21D_PARAMS_H

#include "board.h"
#include "htu21d.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef HTU21D_PARAM_DEV
#define HTU21D_PARAM_DEV          I2C_DEV(0)
#endif
#ifndef HTU21D_PARAM_ADDR
#define HTU21D_PARAM_ADDR         (HTU21D_I2C_ADDRESS)
#endif
#ifndef HTU21D_PARAM_RES
#define HTU21D_PARAM_RES          (HTU21D_RES_RH12_T14)
#endif

#ifndef HTU21D_PARAMS
#define HTU21D_PARAMS             { \
                                     .dev  = HTU21D_PARAM_DEV,  \
                                     .addr = HTU21D_PARAM_ADDR, \
                                     .res  = HTU21D_PARAM_RES, \
                                   }
#endif

#ifndef HTU21D_SAUL_INFO
#define HTU21D_SAUL_INFO          { .name = "htu21d" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const htu21d_params_t htu21d_params[] =
{
    HTU21D_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t htu21d_saul_info[] =
{
    HTU21D_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* HTU21D_PARAMS_H */
/** @} */
