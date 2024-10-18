/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_opt3001
 *
 * @{
 * @file
 * @brief       Default configuration for OPT3001 devices
 *
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */

#ifndef OPT3001_PARAMS_H
#define OPT3001_PARAMS_H

#include "board.h"
#include "opt3001.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Set default configuration parameters for the opt3001
 * @ingroup config
 * @{
 */
 #ifndef OPT3001_PARAM_I2C_DEV
 #define OPT3001_PARAM_I2C_DEV    I2C_DEV(0)
 #endif
 #ifndef OPT3001_PARAM_I2C_ADDR
 #define OPT3001_PARAM_I2C_ADDR    (CONFIG_OPT3001_I2C_ADDRESS)
 #endif

 #ifndef OPT3001_PARAMS
 #define OPT3001_PARAMS    { .i2c_dev  = OPT3001_PARAM_I2C_DEV,  \
                             .i2c_addr = OPT3001_PARAM_I2C_ADDR}

 #endif
 #ifndef OPT3001_SAUL_INFO
 #define OPT3001_SAUL_INFO    { .name = "opt3001" }
 #endif
 /**@}*/

 /**
  * @brief   OPT3001 configuration
  */
 static const opt3001_params_t opt3001_params[] =
 {
     OPT3001_PARAMS
 };

 /**
  * @brief   Additional meta information to keep in the SAUL registry
  */
 static const saul_reg_info_t opt3001_saul_info[] =
 {
     OPT3001_SAUL_INFO
 };

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* OPT3001_PARAMS_H */
