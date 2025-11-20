/*
 * Copyright (C) 2020 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_cst816s
 *
 * @{
 * @file
 *
 * @brief       Default configuration for the CST816S touch screen driver
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include "board.h"
#include "cst816s.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the CST816S
 * @{
 */
/* I2C configuration */
#ifndef CST816S_PARAM_I2C_DEV
#define CST816S_PARAM_I2C_DEV        I2C_DEV(0)
#endif

#ifndef CST816S_PARAM_I2C_ADDR
#define CST816S_PARAM_I2C_ADDR       (0x15)
#endif

#ifndef CST816S_PARAM_IRQ
#define CST816S_PARAM_IRQ            GPIO_PIN(0, 28)
#endif

#ifndef CST816S_PARAM_IRQ_FLANK
#define CST816S_PARAM_IRQ_FLANK      GPIO_FALLING
#endif

#ifndef CST816S_PARAM_RESET
#define CST816S_PARAM_RESET          GPIO_PIN(0, 10)
#endif

#define CST816S_PARAMS                      \
    {                                       \
        .i2c_dev   = CST816S_PARAM_I2C_DEV,  \
        .i2c_addr  = CST816S_PARAM_I2C_ADDR, \
        .irq       = CST816S_PARAM_IRQ,       \
        .irq_flank = CST816S_PARAM_IRQ_FLANK, \
        .reset     = CST816S_PARAM_RESET,    \
    }
/**@}*/

/**
 * @brief   Configure CST816S
 */
static const cst816s_params_t cst816s_params[] =
{
   CST816S_PARAMS
};

/**
 * @brief   The number of configured sensors
 */
#define CST816S_NUMOF    ARRAY_SIZE(cst816s_params)

/**
 * @brief   Default screen identifiers
 */
#ifndef CST816S_PARAM_SCREEN_IDS
#define CST816S_PARAM_SCREEN_IDS       0
#endif

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t cst816s_screen_ids[] =
{
    CST816S_PARAM_SCREEN_IDS,
};

#ifdef __cplusplus
}
#endif

/** @} */
