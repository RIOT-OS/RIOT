/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Timer CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_TIMER_H
#define PERIPH_CPU_TIMER_H

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   All STM timers have at most 4 capture-compare channels
 */
#define TIMER_CHANNEL_NUMOF (4U)

/**
 * @brief   The driver provides a relative set function
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief   Define a macro for accessing a timer channel
 */
#define TIM_CHAN(tim, chan) *(&dev(tim)->CCR1 + chan)

/**
 * @brief   Timer configuration
 */
typedef struct {
    periph_base_t base;     /**< peripheral base */
    TIM_TypeDef *dev;       /**< timer device */
    uint32_t max;           /**< maximum value to count to (16/32 bit) */
    uint8_t bus;            /**< APBx bus the timer is clock from */
    uint8_t irqn;           /**< global IRQ channel */
    uint8_t channel_numof;  /**< number of channels, 0 is alias for
                                 @ref TIMER_CHANNEL_NUMOF */
} timer_conf_t;

/**
 * @brief   Base peripheral definition for timer 1.
 */
#ifdef TIM1
static const periph_base_t periph_timer1 = {
    #if   defined(RCC_APBENR2_TIM1EN)
    .en_reg             = &RCC->APBENR2,
    .en_mask            = RCC_APBENR2_TIM1EN,
    #elif defined(RCC_APB2ENR_TIM1EN)
    .en_reg             = &RCC->APB2ENR,
    .en_mask            = RCC_APB2ENR_TIM1EN,
    #else
/*    # warning "base not defined"*/
    #endif
};
#endif

/**
 * @brief   Base peripheral definition for timer 2.
 */
#ifdef TIM2
static const periph_base_t periph_timer2 = {
    #if   defined(RCC_APB1ENR_TIM2EN)
    .en_reg             = &RCC->APB1ENR,
    .en_mask            = RCC_APB1ENR_TIM2EN,
    #elif defined(RCC_APB1ENR1_TIM2EN)
    .en_reg             = &RCC->APB1ENR1,
    .en_mask = RCC_APB1ENR1_TIM2EN,
    #elif defined(RCC_MC_APB1ENSETR_TIM2EN)
    .en_reg             = &RCC->MC_APB1ENSETR,
    .en_mask            = RCC_MC_APB1ENSETR_TIM2EN,
    #else
/*    # warning "base not defined"*/
    #endif
};
#endif

/**
 * @brief   Base peripheral definition for timer 3.
 */
#ifdef TIM3
static const periph_base_t periph_timer3 = {
    #if   defined(RCC_APBENR1_TIM3EN)
    .en_reg             = &RCC->APBENR1,
    .en_mask            = RCC_APBENR1_TIM3EN,
    #elif defined(RCC_APB1ENR_TIM3EN)
    .en_reg             = &RCC->APB1ENR,
    .en_mask            = RCC_APB1ENR_TIM3EN,
    #elif defined(RCC_APB1ENR1_TIM3EN)
    .en_reg             = &RCC->APB1ENR1,
    .en_mask            = RCC_APB1ENR1_TIM3EN,
    #else
/*    # warning "base not defined"*/
    #endif
};
#endif

/**
 * @brief   Base peripheral definition for timer 3.
 */
#ifdef TIM4
static const periph_base_t periph_timer4 = {
    #if   defined(RCC_APB1ENR_TIM4EN)
    .en_reg             = &RCC->APB1ENR,
    .en_mask            = RCC_APB1ENR_TIM4EN,
    #else
/*    # warning "base not defined"*/
    #endif
};
#endif

/**
 * @brief   Base peripheral definition for timer 5.
 */
#ifdef TIM5
static const periph_base_t periph_timer5 = {
    #if   defined(RCC_APB1ENR_TIM5EN)
    .en_reg             = &RCC->APB1ENR,
    .en_mask            = RCC_APB1ENR_TIM5EN,
    #elif defined(RCC_APB1ENR1_TIM5EN)
    .en_reg             = &RCC->APB1ENR1,
    .en_mask            = RCC_APB1ENR1_TIM5EN,
    #else
/*    # warning "base not defined"*/
    #endif
};
#endif

/**
 * @brief   Base peripheral definition for timer 15.
 */
#ifdef TIM15
static const periph_base_t periph_timer15 = {
    #if   defined(RCC_APB2ENR_TIM15EN)
    .en_reg             = &RCC->APB2ENR,
    .en_mask            = RCC_APB2ENR_TIM15EN,
    #else
/*    # warning "base not defined"*/
    #endif
};
#endif

/**
 * @brief   Base peripheral definition for timer 16.
 */
#ifdef TIM16
static const periph_base_t periph_timer16 = {
    #if   defined(RCC_APB2ENR_TIM16EN)
    .en_reg             = &RCC->APB2ENR,
    .en_mask            = RCC_APB2ENR_TIM16EN,
    #else
/*    # warning "base not defined"*/
    #endif
};
#endif

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_TIMER_H */
/** @} */
