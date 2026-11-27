/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Common configuration for STM32 Timer peripheral based on TIM2
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Timer configuration
 * @{
 */
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#  define TIMER_0_MAX_VALUE   (0x0000ffff) /**< max value */
#else
#  define TIMER_0_MAX_VALUE   (0xffffffff) /**< max value */
#endif

static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .max      = TIMER_0_MAX_VALUE,
#if defined(RCC_APB1ENR1_TIM2EN)
        .rcc_mask = RCC_APB1ENR1_TIM2EN,
#elif defined(RCC_MC_APB1ENSETR_TIM2EN)
        .rcc_mask = RCC_MC_APB1ENSETR_TIM2EN,
#else
        .rcc_mask = RCC_APB1ENR_TIM2EN,
#endif
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
