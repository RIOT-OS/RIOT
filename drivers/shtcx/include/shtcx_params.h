/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_shtc1
 *
 * @{
 *
 * @file
 * @brief       Default parameters for the SHTC1 Temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */

#ifndef SHTC1_PARAMS_H
#define SHTC1_PARAMS_H

#include "board.h"
#include "shtc1.h"
#include "shtc1_regs.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for SHTC1 sensors
 * @{
 */
#ifndef SHTC1_PARAM_I2C_DEV
#define SHTC1_PARAM_I2C_DEV        (I2C_DEV(0))
#endif
#ifndef SHTC1_PARAM_I2C_ADDR
#define SHTC1_PARAM_I2C_ADDR       SHTC1_I2C_ADDRESS
#endif

#ifndef SHTC1_PARAMS
#define SHTC1_PARAMS           { .i2c_dev = SHTC1_PARAM_I2C_DEV, \
                                 .i2c_addr = SHTC1_PARAM_I2C_ADDR }
#endif
#ifndef SHTC1_SAUL_INFO
#define SHTC1_SAUL_INFO             { .name = "shtc1 temperature" }, \
                                    { .name = "shtc1 humidity" }
#endif
/** @} */

/**
 * @brief   Allocation of SHTC1 configuration
 */
static const shtc1_params_t shtc1_params[] =
{
    SHTC1_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t shtc1_saul_info[] =
{
    SHTC1_SAUL_INFO
};
#ifdef __cplusplus
}
#endif

#endif /* SHTC1_PARAMS_H */
/** @} */
