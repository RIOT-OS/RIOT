/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_msp430
 * @{
 *
 * @file
 * @brief       Common timer configuration for TIMER_A clocked by SMCLK and
 *              TIMER_B clocked by ACLK
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
/**
 * @brief   Timer configuration
 */
static const timer_conf_t timer_conf[] = {
    {
        .timer = &TIMER_A,
        .irq_flags = &TIMER_A_IRQFLAGS,
        .clock_source = TIMER_CLOCK_SOURCE_SUBMAIN_CLOCK,
    },
    {
        .timer = &TIMER_B,
        .irq_flags = &TIMER_B_IRQFLAGS,
        .clock_source = TIMER_CLOCK_SOURCE_AUXILIARY_CLOCK,
    }
};
#define TIMER_NUMOF         ARRAY_SIZE(timer_conf)  /**< Number of timers available */

#define TIMER0_ISR_CC0      (TIMERA0_VECTOR)        /**< IRQ vector for channel 0 of TIMER_DEV(0) */
#define TIMER0_ISR_CCX      (TIMERA1_VECTOR)        /**< IRQ vector for channels !=0 of TIMER_DEV(0) */
#define TIMER1_ISR_CC0      (TIMERB0_VECTOR)        /**< IRQ vector for channel 0 of TIMER_DEV(0) */
#define TIMER1_ISR_CCX      (TIMERB1_VECTOR)        /**< IRQ vector for channels !=0 of TIMER_DEV(1) */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
