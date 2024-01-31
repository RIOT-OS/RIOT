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
 * @brief       Default timer configuration for GD32F310 boards
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef CFG_TIMER_DEFAULT_H
#define CFG_TIMER_DEFAULT_H

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
        .dev      = TIMER0,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER0EN_Msk,
        .bus      = APB2,
        // .irqn     = TIMER0_Channel_IRQn
    },
    {
        .dev      = TIMER2,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER2EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER5,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER5EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER13,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER13EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER14,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER14EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER15,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER15EN_Msk,
        .bus      = APB2,
    },
    {
        .dev      = TIMER16,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER16EN_Msk,
        .bus      = APB2,
    },
};

#define TIMER_0_ISR         isr_tim1_cc

#define TIMER_0_IRQN        TIMER0_Channel_IRQn
#define TIMER_2_IRQN        TIMER2_IRQn
#define TIMER_5_IRQN        TIMER5_IRQn
#define TIMER_13_IRQN       TIMER13_IRQn
#define TIMER_14_IRQN       TIMER14_IRQn
#define TIMER_15_IRQN       TIMER15_IRQn
#define TIMER_16_IRQN       TIMER16_IRQn

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_TIMER_DEFAULT_H */
/** @} */
