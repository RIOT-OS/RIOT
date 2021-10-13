/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nrf52
 * @{
 *
 * @file
 * @brief       Default timer configuration for nRF52 based boards
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef CFG_TIMER_DEFAULT_H
#define CFG_TIMER_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = NRF_TIMER1,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER1_IRQn
    },
    {
        .dev      = NRF_TIMER2,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_08Bit,
        .irqn     = TIMER2_IRQn
    },
    /* The later timers are only present on the larger NRF52 CPUs like NRF52840
     * or NRF52833, but not small ones like NRF52810 */
#ifdef NRF_TIMER3
    {
        .dev      = NRF_TIMER3,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_08Bit,
        .irqn     = TIMER3_IRQn
    },
#endif
#ifdef NRF_TIMER4
    {
        .dev      = NRF_TIMER4,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_08Bit,
        .irqn     = TIMER4_IRQn
    }
#endif
};

#define TIMER_0_ISR         isr_timer1
#define TIMER_1_ISR         isr_timer2
#define TIMER_2_ISR         isr_timer3
#define TIMER_3_ISR         isr_timer4

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_TIMER_DEFAULT_H */
/** @} */
