/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ccs811
 *
 * @{
 * @file
 * @brief       Default configuration for CCS811
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CCS811_PARAMS_H
#define CCS811_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "ccs811.h"
#include "ccs811_constants.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the CCS811
 * @{
 */
#ifndef CCS811_PARAM_I2C_DEV
#define CCS811_PARAM_I2C_DEV        I2C_DEV(0)
#endif
#ifndef CCS811_PARAM_I2C_ADDR
#define CCS811_PARAM_I2C_ADDR       CCS811_ADDR
#endif
#ifndef CCS811_PARAM_WAKE_PIN
#define CCS811_PARAM_WAKE_PIN       GPIO_UNDEF /* wake pin directly connected to ground */
#endif
#ifndef CCS811_PARAMS
#define CCS811_PARAMS               { .i2c_dev      = CCS811_PARAM_I2C_DEV,   \
                                      .i2c_addr     = CCS811_PARAM_I2C_ADDR,  \
                                      .wake_pin     = CCS811_PARAM_WAKE_PIN }
#endif
#ifndef CCS811_SAUL_INFO
#define CCS811_SAUL_INFO            { .name = "ccs811" }
#endif
/**@}*/

/**
 * @brief   Configure CCS811
 */
static const ccs811_params_t ccs811_params[] =
{
    CCS811_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
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
