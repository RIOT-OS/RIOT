/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht3x
 * @brief       Default configuration for Sensirion SHT30/SHT31/SHT35 devices
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef SHT3X_PARAMS_H
#define SHT3X_PARAMS_H

#include "board.h"
#include "sht3x.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SHT3x default configuration parameters
 * @{
 */
#ifndef SHT3X_PARAM_I2C_DEV
#define SHT3X_PARAM_I2C_DEV     (I2C_DEV(0))
#endif
#ifndef SHT3X_PARAM_I2C_ADDR
#define SHT3X_PARAM_I2C_ADDR    (SHT3X_I2C_ADDR_2)
#endif
#ifndef SHT3X_PARAM_MODE
#define SHT3X_PARAM_MODE        (SHT3X_PERIODIC_2_MPS)
#endif
#ifndef SHT3X_PARAM_REPEAT
#define SHT3X_PARAM_REPEAT      (SHT3X_HIGH)
#endif

#ifndef SHT3X_PARAMS
#define SHT3X_PARAMS    { .i2c_dev  = SHT3X_PARAM_I2C_DEV,  \
                          .i2c_addr = SHT3X_PARAM_I2C_ADDR, \
                          .mode     = SHT3X_PARAM_MODE,     \
                          .repeat   = SHT3X_PARAM_REPEAT    \
                        }
#endif

#ifndef SHT3X_SAUL_INFO
#define SHT3X_SAUL_INFO { .name = "sht3x1" }
#endif
/**@}*/

/**
 * @brief   SHT3x configuration
 */
static const sht3x_params_t sht3x_params[] =
{
    SHT3X_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t sht3x_saul_info[] =
{
    SHT3X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* SHT3X_PARAMS_H */
/** @} */
