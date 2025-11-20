/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_hmc5883l
 * @brief       Default configuration for the Honeywell HMC5883L 3-axis digital compass
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include "board.h"
#include "hmc5883l.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef HMC5883L_PARAM_DEV
#define HMC5883L_PARAM_DEV          I2C_DEV(0)
#endif
#ifndef HMC5883L_PARAM_DOR
#define HMC5883L_PARAM_DOR          (HMC5883L_DOR_15)
#endif
#ifndef HMC5883L_PARAM_MEAS_MODE
#define HMC5883L_PARAM_MEAS_MODE    (HMC5883L_MEAS_MODE_NORMAL)
#endif
#ifndef HMC5883L_PARAM_MEAS_AVG
#define HMC5883L_PARAM_MEAS_AVG     (HMC5883L_MEAS_AVG_NONE)
#endif
#ifndef HMC5883L_PARAM_OP_MODE
#define HMC5883L_PARAM_OP_MODE      (HMC5883L_OP_MODE_CONTINUOUS)
#endif
#ifndef HMC5883L_PARAM_GAIN
#define HMC5883L_PARAM_GAIN         (HMC5883L_GAIN_1090)
#endif
#ifndef HMC5883L_PARAM_INT_PIN
#define HMC5883L_PARAM_INT_PIN      (GPIO_UNDEF)
#endif

#ifdef MODULE_HMC5883L_INT
#ifndef HMC5883L_PARAMS
#define HMC5883L_PARAMS             { \
                                     .dev  = HMC5883L_PARAM_DEV,  \
                                     .dor  = HMC5883L_PARAM_DOR,  \
                                     .gain = HMC5883L_PARAM_GAIN, \
                                     .int_pin = HMC5883L_PARAM_INT_PIN, \
                                     .meas_mode = HMC5883L_PARAM_MEAS_MODE, \
                                     .meas_avg  = HMC5883L_PARAM_MEAS_AVG,  \
                                     .op_mode   = HMC5883L_PARAM_OP_MODE,   \
                                   }
#endif /* HMC5883L_PARAMS */
#else /* MODULE_HMC5883L_INT */
#define HMC5883L_PARAMS             { \
                                     .dev  = HMC5883L_PARAM_DEV,  \
                                     .dor  = HMC5883L_PARAM_DOR,  \
                                     .gain = HMC5883L_PARAM_GAIN, \
                                     .meas_mode = HMC5883L_PARAM_MEAS_MODE, \
                                     .meas_avg  = HMC5883L_PARAM_MEAS_AVG,  \
                                     .op_mode   = HMC5883L_PARAM_OP_MODE,   \
                                   }
#endif /* MODULE_HMC5883L_INT */

#ifndef HMC5883L_SAUL_INFO
#define HMC5883L_SAUL_INFO          { .name = "hmc5883l" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const hmc5883l_params_t hmc5883l_params[] =
{
    HMC5883L_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t hmc5883l_saul_info[] =
{
    HMC5883L_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
