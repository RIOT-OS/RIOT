/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *               2023 Gunar Schorcht <gunar@schorcht.net>
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
 * @brief       Default timer configuration for GD32VF103 boards
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef CFG_TIMER_DEFAULT_H
#define CFG_TIMER_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Timer configuration
 *
 * All GD32VF103xx variants have at least one advanced timer TIMER0 and two
 * general timers TIMER1 and TIMER2. GD32VF10x8 and GD32VF10xB have two
 * additional general timers TIMER3 and TIMER4.
 *
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIMER0,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB2EN_TIMER0EN_Msk,
        .bus      = APB2,
        .irqn     = TIMER0_Channel_IRQn
    },
    {
        .dev      = TIMER1,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER1EN_Msk,
        .bus      = APB1,
        .irqn     = TIMER1_IRQn
    },
#if !defined(MODULE_PERIPH_PM)
    {
        .dev      = TIMER2,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER2EN_Msk,
        .bus      = APB1,
        .irqn     = TIMER2_IRQn
    },
#if defined(CPU_MODEL_GD32VF103C8T6) || defined(CPU_MODEL_GD32VF103CBT6) || \
    defined(CPU_MODEL_GD32VF103R8T6) || defined(CPU_MODEL_GD32VF103RBT6) || \
    defined(CPU_MODEL_GD32VF103T8U6) || defined(CPU_MODEL_GD32VF103TBU6) || \
    defined(CPU_MODEL_GD32VF103V8T6) || defined(CPU_MODEL_GD32VF103VBT6)
    {
        .dev      = TIMER3,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER3EN_Msk,
        .bus      = APB1,
        .irqn     = TIMER3_IRQn
    },
    {
        .dev      = TIMER4,
        .max      = 0x0000ffff,
        .rcu_mask = RCU_APB1EN_TIMER4EN_Msk,
        .bus      = APB1,
        .irqn     = TIMER4_IRQn
    }
#endif
#endif /* !defined(MODULE_PERIPH_PWM) */
};

#define TIMER_0_IRQN        TIMER0_Channel_IRQn
#define TIMER_1_IRQN        TIMER1_IRQn

#if !defined(MODULE_PERIPH_PWM)
#define TIMER_2_IRQN        TIMER2_IRQn
#if defined(CPU_MODEL_GD32VF103C8T6) || defined(CPU_MODEL_GD32VF103CBT6) || \
    defined(CPU_MODEL_GD32VF103R8T6) || defined(CPU_MODEL_GD32VF103RBT6) || \
    defined(CPU_MODEL_GD32VF103T8U6) || defined(CPU_MODEL_GD32VF103TBU6) || \
    defined(CPU_MODEL_GD32VF103V8T6) || defined(CPU_MODEL_GD32VF103VBT6)
#define TIMER_3_IRQN        TIMER3_IRQn
#define TIMER_4_IRQN        TIMER4_IRQn
#endif
#endif /* !defined(MODULE_PERIPH_PWM) */

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_TIMER_DEFAULT_H */
/** @} */
