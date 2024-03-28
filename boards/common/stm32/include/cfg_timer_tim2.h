/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Common configuration for STM32 Timer peripheral based on TIM2
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CFG_TIMER_TIM2_H
#define CFG_TIMER_TIM2_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
        .max      = 0x0000ffff,
#else
        .max      = 0xffffffff,
#endif
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32WL) || defined(CPU_FAM_STM32G4)
        .rcc_mask = RCC_APB1ENR1_TIM2EN,
#elif CPU_FAM_STM32MP1
        .rcc_mask = RCC_MC_APB1ENSETR_TIM2EN,
#else
        .rcc_mask = RCC_APB1ENR_TIM2EN,
#endif
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    }
};

/**
 * @brief   Timer capture configuration for above timer
 */
static const timer_capture_conf_t timer_capture_config[] = {
    {
        .inputs = {
            {
                .pin = GPIO_PIN(PORT_A, 0),
#ifndef CPU_FAM_STM32F1
                .af = GPIO_AF1,
#endif
            },
            {
                .pin = GPIO_PIN(PORT_A, 1),
#ifndef CPU_FAM_STM32F1
                .af = GPIO_AF1,
#endif
            },
            {
                .pin = GPIO_PIN(PORT_A, 2),
#ifndef CPU_FAM_STM32F1
                .af = GPIO_AF1,
#endif
            },
            {
                .pin = GPIO_PIN(PORT_A, 3),
#ifndef CPU_FAM_STM32F1
                .af = GPIO_AF1,
#endif
            },
        }
    }
};

#define TIMER_0_ISR         isr_tim2

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_TIMER_TIM2_H */
/** @} */
