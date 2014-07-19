/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_imx6
 * @{
 *
 * @file            hwtimer_cpu.h
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#include "periph_conf.h"

/**
 * @name Hardware timer configuration
 * @{
 */

#if TIMER_0_EN == 1
  #define PROXY_TIMER_0_CHANNELS TIMER_0_CHANNELS
#else
  #define PROXY_TIMER_0_CHANNELS 0
#endif
#if TIMER_1_EN == 1
  #define PROXY_TIMER_1_CHANNELS TIMER_1_CHANNELS
#else
  #define PROXY_TIMER_1_CHANNELS 0
#endif
#if TIMER_2_EN == 1
  #define PROXY_TIMER_2_CHANNELS TIMER_2_CHANNELS
#else
  #define PROXY_TIMER_2_CHANNELS 0
#endif

/**< the CPU implementation supports 3 HW timers */
#define HWTIMER_MAXTIMERS (PROXY_TIMER_0_CHANNELS + PROXY_TIMER_1_CHANNELS + PROXY_TIMER_2_CHANNELS)


#define HWTIMER_SPEED       1000000         /**< the HW timer runs with 1MHz */
#define HWTIMER_MAXTICKS    (0xFFFFFFFF)    /**< 32-bit timer */
/** @} */


#endif /* __HWTIMER_CPU_H */
/** @} */
