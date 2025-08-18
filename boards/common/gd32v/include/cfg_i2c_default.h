/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht <gunar@schorcht.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_gd32v
 * @{
 *
 * @file
 * @brief       Default I2C configuration for GD32VF103 boards
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

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
 * @brief   Default I2C device configuration
 *
 * The default I2C device configuration allows to define up to two I2C devices
 * `I2C_DEV(0)` and `I2C_DEV(1)`. `I2C_DEV(0)` is always defined if the I2C
 * peripheral is enabled by the module `periph_spi`. The second I2C device
 * `I2C_DEV(1)` is only defined if `I2C_DEV_1_USED` is set to 1 by the board.
 * This allows to use the default configuration with one or two I2C devices
 * depending on whether other peripherals are enabled that would collide with
 * the I2C devices.
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
#if I2C_DEV_1_USED
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 10),
        .sda_pin        = GPIO_PIN(PORT_B, 11),
        .rcu_mask       = RCU_APB1EN_I2C1EN_Msk,
        .irqn           = I2C1_EV_IRQn,
    }
#endif
};

#define I2C_NUMOF   ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
