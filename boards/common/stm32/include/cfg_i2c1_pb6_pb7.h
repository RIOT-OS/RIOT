/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Common configuration for STM32 I2C
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 6),
        .sda_pin        = GPIO_PIN(PORT_B, 7),
#if CPU_FAM_STM32L4
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
#else /* CPU_FAM_STM32L0 */
        .scl_af         = GPIO_AF1,
        .sda_af         = GPIO_AF1,
#endif
        .bus            = APB1,
#if CPU_FAM_STM32L4
        .rcc_mask       = RCC_APB1ENR1_I2C1EN,
        .rcc_sw_mask    = RCC_CCIPR_I2C1SEL_1,          /* HSI (16 MHz) */
        .irqn           = I2C1_ER_IRQn,
#else /* CPU_FAM_STM32L0 */
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .irqn           = I2C1_IRQn
#endif
    }
};

#if CPU_FAM_STM32L4
#define I2C_0_ISR           isr_i2c1_er
#else /* CPU_FAM_STM32L0 */
#define I2C_0_ISR           isr_i2c1
#endif

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */
#ifdef __cplusplus
}
#endif

/** @} */
