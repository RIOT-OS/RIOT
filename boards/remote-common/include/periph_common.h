/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_remote
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Re-Mote boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 */

#ifndef PERIPH_COMMON_H_
#define PERIPH_COMMON_H_

#include "cc2538_gpio.h"
#include "periph_cpu.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_CORECLOCK     (32000000U) /* 32MHz */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (4U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_2_EN          1
#define TIMER_3_EN          1

#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         GPTIMER0
#define TIMER_0_CHANNELS    1
#define TIMER_0_MAX_VALUE   0xffffffff
#define TIMER_0_IRQn_1      GPTIMER_0A_IRQn
#define TIMER_0_IRQn_2      GPTIMER_0B_IRQn
#define TIMER_0_ISR_1       isr_timer0_chan0
#define TIMER_0_ISR_2       isr_timer0_chan1

/* Timer 1 configuration */
#define TIMER_1_DEV         GPTIMER1
#define TIMER_1_CHANNELS    1
#define TIMER_1_MAX_VALUE   0xffffffff
#define TIMER_1_IRQn_1      GPTIMER_1A_IRQn
#define TIMER_1_IRQn_2      GPTIMER_1B_IRQn
#define TIMER_1_ISR_1       isr_timer1_chan0
#define TIMER_1_ISR_2       isr_timer1_chan1

/* Timer 2 configuration */
#define TIMER_2_DEV         GPTIMER2
#define TIMER_2_CHANNELS    1
#define TIMER_2_MAX_VALUE   0xffffffff
#define TIMER_2_IRQn_1      GPTIMER_2A_IRQn
#define TIMER_2_IRQn_2      GPTIMER_2B_IRQn
#define TIMER_2_ISR_1       isr_timer2_chan0
#define TIMER_2_ISR_2       isr_timer2_chan1

/* Timer 3 configuration */
#define TIMER_3_DEV         GPTIMER3
#define TIMER_3_CHANNELS    1
#define TIMER_3_MAX_VALUE   0xffffffff
#define TIMER_3_IRQn_1      GPTIMER_3A_IRQn
#define TIMER_3_IRQn_2      GPTIMER_3B_IRQn
#define TIMER_3_ISR_1       isr_timer3_chan0
#define TIMER_3_ISR_2       isr_timer3_chan1
/** @} */

/**
 * @name Radio peripheral configuration
 * @{
 */
#define RADIO_IRQ_PRIO      1
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_COMMON_H_ */
/** @} */
