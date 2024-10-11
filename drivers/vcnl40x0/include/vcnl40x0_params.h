/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vcnl40x0
 *
 * @{
 * @file
 * @brief       Default configuration for VCNL40X0 devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef VCNL40X0_PARAMS_H
#define VCNL40X0_PARAMS_H

#include "board.h"
#include "vcnl40x0.h"
#include "vcnl40x0_internals.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for the VCNL40X0
 * @{
 */
#ifndef VCNL40X0_PARAM_I2C_DEV
#define VCNL40X0_PARAM_I2C_DEV          I2C_DEV(0)
#endif
#ifndef VCNL40X0_PARAM_I2C_ADDR
#define VCNL40X0_PARAM_I2C_ADDR         (VCNL40X0_ADDR)
#endif
#ifndef VCNL40X0_PARAM_LED_CURRENT
#define VCNL40X0_PARAM_LED_CURRENT      (2U)
#endif
#ifndef VCNL40X0_PARAM_PROXIMITY_RATE
#define VCNL40X0_PARAM_PROXIMITY_RATE   (VCNL40X0_PROXIMITY_RATE_2)
#endif
#ifndef VCNL40X0_PARAM_AMBIENT_AVG
#define VCNL40X0_PARAM_AMBIENT_AVG      (VCNL40X0_AMBIENT_AVERAGE_32)
#endif
#ifndef VCNL40X0_PARAM_AMBIENT_RATE
#define VCNL40X0_PARAM_AMBIENT_RATE     (VCNL40X0_AMBIENT_RATE_2)
#endif

#define VCNL40X0_PARAMS             { .i2c_dev = VCNL40X0_PARAM_I2C_DEV,               \
                                      .i2c_addr = VCNL40X0_PARAM_I2C_ADDR,             \
                                      .led_current = VCNL40X0_PARAM_LED_CURRENT,       \
                                      .proximity_rate = VCNL40X0_PARAM_PROXIMITY_RATE, \
                                      .ambient_avg = VCNL40X0_PARAM_AMBIENT_AVG,       \
                                      .ambient_rate = VCNL40X0_PARAM_AMBIENT_RATE }
/**@}*/

/**
 * @brief   Configure VCNL40X0
 */
static const vcnl40x0_params_t vcnl40x0_params[] =
{
    VCNL40X0_PARAMS,
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t vcnl40x0_saul_reg_info[] =
{
    { .name = "vcnl40x0" }
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* VCNL40X0_PARAMS_H */
