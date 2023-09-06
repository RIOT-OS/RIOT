/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32f310c-eval
 * @{
 *
 * @file
 * @brief       Default I2C configuration for GD32F310 boards
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef CFG_I2C_DEFAULT_H
#define CFG_I2C_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    I2C configuration
 *
 * @note    This board may require external pullup resistors for i2c operation.
 * @{
 */

/**
 * @brief   Disable the second I2C device `I2C_DEV(1)` by default
 *
 * The second I2C device `I2C_DEV(1)` is only defined if `I2C_DEV_1_USED`
 * is set to 1 by the board.
 * This allows to use the default configuration with one or two I2C devices
 * depending on whether other peripherals are enabled that would collide with
 * the I2C devices.
 */
#ifndef I2C_DEV_1_USED
#define I2C_DEV_1_USED  0
#endif

/**
 * @brief   Timing register settings
 *
 * @ref i2c_timing_param_t
 */
static const i2c_timing_param_t timing_params[] = {
    [ I2C_SPEED_NORMAL ]    = {
        .presc  = 3,
        .scll   = 0x13,     /* t_SCLL   = 5.0us  */
        .sclh   = 0xF,      /* t_SCLH   = 4.0us  */
        .sdadel = 0x2,      /* t_SDADEL = 500ns  */
        .scldel = 0x4,      /* t_SCLDEL = 1250ns */
    },
    [ I2C_SPEED_FAST ]      = {
        .presc  = 1,
        .scll   = 0x9,      /* t_SCLL   = 1250ns */
        .sclh   = 0x3,      /* t_SCLH   = 500ns  */
        .sdadel = 0x2,      /* t_SDADEL = 250ns  */
        .scldel = 0x3,      /* t_SCLDEL = 500ns  */
    },
    [ I2C_SPEED_FAST_PLUS ] = {
        .presc =  0,
        .scll =   0x4,      /* t_SCLL   = 312.5ns */
        .sclh =   0x2,      /* t_SCLH   = 187.5ns */
        .sdadel = 0x0,      /* t_SDADEL = 0ns     */
        .scldel = 0x2,      /* t_SCLDEL = 187.5ns */
    }
};

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C0,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 6),
        .sda_pin        = GPIO_PIN(PORT_B, 7),
        // .scl_af         = GPIO_AF_1,
        // .sda_af         = GPIO_AF_1,
        .bus            = APB1,
        .rcu_mask       = RCU_APB1EN_I2C0EN_Msk,// RCU_APB1ENR_I2C1EN,
        // .rcu_sw_mask    = RCU_CFGR3_I2C1SW,
        .irqn           = I2C0_EV_IRQn, //or I2C0_ER_IRQn
    },
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 8),
        .sda_pin        = GPIO_PIN(PORT_B, 9),
        // .scl_af         = GPIO_AF_1,
        // .sda_af         = GPIO_AF_1,
        .bus            = APB1,
        .rcu_mask       = RCU_APB1EN_I2C1EN_Msk,
        // .rcu_sw_mask    = ,
        .irqn           = I2C1_EV_IRQn,
    }
};

#define I2C_NUMOF   ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_I2C_DEFAULT_H */
/** @} */
