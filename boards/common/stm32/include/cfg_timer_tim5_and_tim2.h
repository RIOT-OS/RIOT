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
 * @brief       Common configuration for STM32 Timer peripheral based on TIM5
 *              and TIM2
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
static const timer_conf_t timer_config[] = {
    /* intentionally not sorted alphabetically: E.g. on STM32L1 TIM5 is
     * 32 bit while TIM2 is only 16 bit. ztimer defaults to the first timer
     * defined and does profit from using a 32 bit timer */
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
#if defined(RCC_APB1ENR1_TIM5EN)
        .rcc_mask = RCC_APB1ENR1_TIM5EN,
#else
        .rcc_mask = RCC_APB1ENR_TIM5EN,
#endif
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    },
    {
        .dev      = TIM2,
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
        .max      = 0x0000ffff,
#else
        .max      = 0xffffffff,
#endif
#if defined(RCC_APB1ENR1_TIM2EN)
        .rcc_mask = RCC_APB1ENR1_TIM2EN,
#elif defined(RCC_MC_APB1ENSETR_TIM2EN)
        .rcc_mask = RCC_MC_APB1ENSETR_TIM2EN,
#else
        .rcc_mask = RCC_APB1ENR_TIM2EN,
#endif
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    },
};

#define TIMER_0_ISR         isr_tim5    /**< IRQ of timer at idx 0 */
#define TIMER_1_ISR         isr_tim2    /**< IRQ of timer at idx 1 */

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
