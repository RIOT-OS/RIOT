/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_tm4c123
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

/**
 * @name Hardware timer configuration
 * @{
 */

#define TIMER_0_PRESCALER   (1 << 10)

#define HWTIMER_MAXTIMERS   (5)                 /**< the CPU implementation supports 6 HW timers */
#define HWTIMER_SPEED       (80000000UL / TIMER_0_PRESCALER) /**< the HW timer runs with 80 MHz */
#define HWTIMER_MAXTICKS    (0xFFFFFFFF)        /**< 32-bit timer */
/** @} */

#endif /* __HWTIMER_CPU_H */
/** @} */
