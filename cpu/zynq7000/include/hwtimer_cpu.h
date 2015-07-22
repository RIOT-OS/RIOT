/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_zynq7000
 * @{
 *
 * @file            hwtimer_cpu.h
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Simon Vincent <simon.vincent@xsilon.com>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Hardware timer configuration
 * @{
 */

/* Timer 0 configuration */
#define TIMER_0_DEV         TTC0
#define TIMER_0_CHANNELS    3
#define TIMER_0_MAX_VALUE   (0xffff) /* 16-bit timer */

/* Timer 1 configuration */
#define TIMER_1_DEV         TTC0
#define TIMER_1_CHANNELS    3
#define TIMER_1_MAX_VALUE   (0xffff) /* 16-bit timer */

/* Timer 2 configuration */
#define TIMER_2_DEV         TTC0
#define TIMER_2_CHANNELS    3
#define TIMER_2_MAX_VALUE   (0xffff) /* 16-bit timer */

/* Timer 3 configuration */
#define TIMER_3_DEV         TTC1
#define TIMER_3_CHANNELS    3
#define TIMER_3_MAX_VALUE   (0xffff) /* 16-bit timer */


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

#if TIMER_3_EN == 1
  #define PROXY_TIMER_3_CHANNELS TIMER_3_CHANNELS
#else
  #define PROXY_TIMER_3_CHANNELS 0
#endif

#define HWTIMER_MAXTIMERS (PROXY_TIMER_0_CHANNELS + PROXY_TIMER_1_CHANNELS + PROXY_TIMER_2_CHANNELS + PROXY_TIMER_3_CHANNELS)

#define HWTIMER_PRESCALER_X (13)                            /* divisor is calculated by 2^(x+1). This gives us min 163us, max 10s period. */
#define HWTIMER_PRESCALER   (1<<(HWTIMER_PRESCALER_X+1))    /* 2^(x+1)*/
#define HWTIMER_SPEED       (F_BUS/HWTIMER_PRESCALER)
#define HWTIMER_MAXTICKS    (0xFFFF)                        /**< 16-bit timer */

#define TIMER_0_OFFSET      0
#define TIMER_1_OFFSET      (TIMER_0_EN * TIMER_0_CHANNELS)
#define TIMER_2_OFFSET      (TIMER_1_OFFSET + (TIMER_1_EN * TIMER_1_CHANNELS))
#define TIMER_3_OFFSET      (TIMER_2_OFFSET + (TIMER_2_EN * TIMER_2_CHANNELS))

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
