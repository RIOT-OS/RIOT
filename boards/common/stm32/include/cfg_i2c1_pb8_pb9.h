/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
        .scl_pin        = GPIO_PIN(PORT_B, 8),
        .sda_pin        = GPIO_PIN(PORT_B, 9),
#if CPU_FAM_STM32F0
        .scl_af         = GPIO_AF1,
        .sda_af         = GPIO_AF1,
#elif CPU_FAM_STM32G0 || CPU_FAM_STM32C0
        .scl_af         = GPIO_AF6,
        .sda_af         = GPIO_AF6,
#else
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
#endif
        .bus            = APB1,
#if CPU_FAM_STM32F4 || CPU_FAM_STM32F2
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C1_EV_IRQn,
#elif CPU_FAM_STM32L4 || CPU_FAM_STM32WB || CPU_FAM_STM32G4
        .rcc_mask       = RCC_APB1ENR1_I2C1EN,
        .rcc_sw_mask    = RCC_CCIPR_I2C1SEL_1,          /* HSI (16 MHz) */
        .irqn           = I2C1_ER_IRQn,
#elif CPU_FAM_STM32L5
        .rcc_mask       = RCC_APB1ENR1_I2C1EN,
        .rcc_sw_mask    = RCC_CCIPR1_I2C1SEL_1,         /* HSI (16 MHz) */
        .irqn           = I2C1_ER_IRQn,
#elif CPU_FAM_STM32G0 || CPU_FAM_STM32C0
        .rcc_mask       = RCC_APBENR1_I2C1EN,
        .rcc_sw_mask    = RCC_CCIPR_I2C1SEL_1,          /* HSI (16 MHz) */
        .irqn           = I2C1_IRQn,
#elif CPU_FAM_STM32F7
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .rcc_sw_mask    = RCC_DCKCFGR2_I2C1SEL_1,       /* HSI (16 MHz) */
        .irqn           = I2C1_ER_IRQn,
#elif CPU_FAM_STM32F0 || CPU_FAM_STM32L0
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
#if CPU_FAM_STM32F0
        .rcc_sw_mask    = RCC_CFGR3_I2C1SW,
#endif
        .irqn           = I2C1_IRQn,
#endif
    }
};

#if CPU_FAM_STM32F4 || CPU_FAM_STM32F2
#define I2C_0_ISR           isr_i2c1_ev
#elif CPU_FAM_STM32L4 || CPU_FAM_STM32F7 || CPU_FAM_STM32WB || CPU_FAM_STM32L5
#define I2C_0_ISR           isr_i2c1_er
#elif CPU_FAM_STM32F0 || CPU_FAM_STM32L0 || CPU_FAM_STM32G0 || CPU_FAM_STM32C0
#define I2C_0_ISR           isr_i2c1
#endif

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
