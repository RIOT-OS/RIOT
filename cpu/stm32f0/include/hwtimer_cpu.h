/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_stm32f0
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS       (4U)            /**< the CPU implementation supports 4 HW timers */
#define HWTIMER_SPEED           (1000000U)      /**< the HW timer runs with 1MHz */
#define HWTIMER_MAXTICKS        (0xFFFFFFFF)    /**< 32-bit timer */
#define HWTIMER_SPIN_BARRIER    (18U)           /**< set spin barrier to at least 18 ticks */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
