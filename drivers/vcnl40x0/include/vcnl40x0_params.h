/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vcnl40x0
 * @brief       Default configuration for Vishay VCNL40X0 proximity and ambient light sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef VCNL40X0_PARAMS_H
#define VCNL40X0_PARAMS_H

#include "board.h"
#include "vcnl40x0.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef VCNL40X0_PARAM_DEV
/** default device is I2C_DEV(0) */
#define VCNL40X0_PARAM_DEV          I2C_DEV(0)
#endif

#ifndef VCNL40X0_PARAM_ALS_RATE
/** default ambient light measurement rate is 2 measurements per second */
#define VCNL40X0_PARAM_ALS_RATE     (VCNL40X0_ALS_RATE_2)
#endif

#ifndef VCNL40X0_PARAM_ALS_AVG
/** default number of conversions used during one measurement cycle is 32 */
#define VCNL40X0_PARAM_ALS_AVG      (VCNL40X0_ALS_AVG_32)
#endif

#ifndef VCNL40X0_PARAM_PRX_RATE
/** default proximity measurement rate is 1.95 measurements per second */
#define VCNL40X0_PARAM_PRX_RATE     (VCNL40X0_PRX_RATE_1_95)
#endif

#ifndef VCNL40X0_PARAM_PRX_IR_LED
/** default IR LED current used for proximity measurement rate is 20 mA */
#define VCNL40X0_PARAM_PRX_IR_LED   (20)
#endif

#ifndef VCNL40X0_PARAM_INT_PIN
/** default interrupt pin */
#define VCNL40X0_PARAM_INT_PIN      (GPIO_PIN(0, 0))
#endif

#ifndef VCNL40X0_PARAMS
#define VCNL40X0_PARAMS             { \
                                        .dev  = VCNL40X0_PARAM_DEV,  \
                                        .als_rate = VCNL40X0_PARAM_ALS_RATE, \
                                        .als_avg = VCNL40X0_PARAM_ALS_AVG, \
                                        .prx_rate = VCNL40X0_PARAM_PRX_RATE, \
                                        .prx_ir_led = VCNL40X0_PARAM_PRX_IR_LED, \
                                        .int_pin = VCNL40X0_PARAM_INT_PIN, \
                                    }
#endif

#ifndef VCNL40X0_SAUL_INFO
#define VCNL40X0_SAUL_INFO          { .name = "vcnl40x0" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const vcnl40x0_params_t vcnl40x0_params[] =
{
    VCNL40X0_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t vcnl40x0_saul_info[] =
{
    VCNL40X0_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* VCNL40X0_PARAMS_H */
/** @} */
