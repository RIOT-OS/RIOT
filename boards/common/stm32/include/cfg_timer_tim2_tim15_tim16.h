/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Common configuration for STM32 Timer peripheral based on TIM2,
 *              TIM15, and TIM16
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef CFG_TIMER_TIM2_TIM15_TIM16_H
#define CFG_TIMER_TIM2_TIM15_TIM16_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Please note: This likely needs some generalization for use in STM32 families
 * other than L4. */
/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev            = TIM2,
        .max            = 0xffffffff,
#if defined(RCC_APB1ENR_TIM2EN)
        .rcc_mask       = RCC_APB1ENR_TIM2EN,
#else
        .rcc_mask       = RCC_APB1ENR1_TIM2EN,
#endif
        .bus            = APB1,
        .irqn           = TIM2_IRQn
    },
    {
        .dev            = TIM15,
        .max            = 0x0000ffff,
        .rcc_mask       = RCC_APB2ENR_TIM15EN,
        .bus            = APB2,
        .irqn           = TIM1_BRK_TIM15_IRQn,
        .channel_numof  = 2,
    },
    {
        .dev            = TIM16,
        .max            = 0x0000ffff,
        .rcc_mask       = RCC_APB2ENR_TIM16EN,
        .bus            = APB2,
        .irqn           = TIM1_UP_TIM16_IRQn,
        .channel_numof  = 1,
    },
};

#define TIMER_0_ISR         isr_tim2            /**< IRQ of timer at idx 0 */
#define TIMER_1_ISR         isr_tim1_brk_tim15  /**< IRQ of timer at idx 1 */
#define TIMER_2_ISR         isr_tim1_up_tim16   /**< IRQ of timer at idx 2 */

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CFG_TIMER_TIM2_TIM15_TIM16_H */
