/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ccs811
 * @brief       Default configuration for AMS CCS811 digital gas sensors
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef CCS811_PARAMS_H
#define CCS811_PARAMS_H

#include "board.h"
#include "ccs811.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    CCS811 default configuration parameters
 * @{
 */
#ifndef CCS811_PARAM_I2C_DEV
#define CCS811_PARAM_I2C_DEV     (I2C_DEV(0))
#endif
#ifndef CCS811_PARAM_I2C_ADDR
#define CCS811_PARAM_I2C_ADDR    (CCS811_I2C_ADDRESS_1)
#endif
#ifndef CCS811_PARAM_MODE
#define CCS811_PARAM_MODE        (CCS811_MODE_1S)
#endif
#ifndef CCS811_PARAM_RESET_PIN
#define CCS811_PARAM_RESET_PIN   (GPIO_UNDEF)
#endif
#ifndef CCS811_PARAM_WAKE_PIN
#define CCS811_PARAM_WAKE_PIN    (GPIO_UNDEF)
#endif
#ifndef CCS811_PARAM_INT_PIN
#define CCS811_PARAM_INT_PIN     (GPIO_PIN(0, 0))
#endif
#ifndef CCS811_PARAM_INT_MODE
#define CCS811_PARAM_INT_MODE    (CCS811_INT_NONE)
#endif

#ifndef CCS811_PARAMS
#ifdef MODULE_CCS811_FULL
#define CCS811_PARAMS    { .i2c_dev   = CCS811_PARAM_I2C_DEV,  \
                           .i2c_addr  = CCS811_PARAM_I2C_ADDR, \
                           .mode      = CCS811_PARAM_MODE,     \
                           .int_pin   = CCS811_PARAM_INT_PIN,  \
                           .int_mode  = CCS811_PARAM_INT_MODE, \
                           .wake_pin  = CCS811_PARAM_WAKE_PIN, \
                           .reset_pin = CCS811_PARAM_RESET_PIN \
                         }
#else
#define CCS811_PARAMS    { .i2c_dev   = CCS811_PARAM_I2C_DEV,  \
                           .i2c_addr  = CCS811_PARAM_I2C_ADDR, \
                           .mode      = CCS811_PARAM_MODE,     \
                           .wake_pin  = CCS811_PARAM_WAKE_PIN, \
                           .reset_pin = CCS811_PARAM_RESET_PIN \
                         }
#endif
#endif
#ifndef CCS811_SAUL_INFO
#define CCS811_SAUL_INFO { .name = "ccs811" }
#endif
/**@}*/

/**
 * @brief   CCS811 configuration
 */
static const ccs811_params_t ccs811_params[] =
{
    CCS811_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t ccs811_saul_info[] =
{
    CCS811_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* CCS811_PARAMS_H */
/** @} */
