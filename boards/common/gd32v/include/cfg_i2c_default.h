/*
 * Copyright (C) 2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_gd32v
 * @{
 *
 * @file
 * @brief       Default I2C configuration for GD32VF103 boards
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
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
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C0,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 6),
        .sda_pin        = GPIO_PIN(PORT_B, 7),
        .rcu_mask       = RCU_APB1EN_I2C0EN_Msk,
        .irqn           = I2C0_EV_IRQn,
    },
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 10),
        .sda_pin        = GPIO_PIN(PORT_B, 11),
        .rcu_mask       = RCU_APB1EN_I2C1EN_Msk,
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
