/*
 * Copyright 2017, RWTH Aachen. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_shtcx
 *
 * @{
 *
 * @file
 * @brief       Default parameters for the SHTCX Temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 */


#include "board.h"
#include "shtcx.h"
#include "shtcx_regs.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for SHTCX sensors
 * @{
 */
#ifndef SHTCX_PARAM_I2C_DEV
#define SHTCX_PARAM_I2C_DEV        (I2C_DEV(0))      /**< Default I2C BUS used */
#endif
#ifndef SHTCX_PARAM_I2C_ADDR
#define SHTCX_PARAM_I2C_ADDR       SHTCX_I2C_ADDRESS /**< I2C Address */
#endif

#ifndef SHTCX_PARAMS
#define SHTCX_PARAMS               { .i2c_dev  = SHTCX_PARAM_I2C_DEV, \
                                     .i2c_addr = SHTCX_PARAM_I2C_ADDR }
#endif
#ifndef SHTCX_SAUL_INFO
#define SHTCX_SAUL_INFO            { .name = "shtcx temperature" }, \
                                   { .name = "shtcx humidity" }
#endif
/** @} */

/**
 * @brief   Allocation of SHTCX configuration
 */
static const shtcx_params_t shtcx_params[] =
{
    SHTCX_PARAMS
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t shtcx_saul_info[] =
{
    SHTCX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
