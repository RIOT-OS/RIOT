/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc3200
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Attilio Dona' <@attiliodona>
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
#define HWTIMER_MAXTIMERS   TIMER_NUM_CHANNELS /**< Number of hwtimers */
#define HWTIMER_SPEED       F_CPU   /**< The hardware timer runs at cpu clock (80MHz) */
#define HWTIMER_MAXTICKS    0xFFFFFFFF         /**< 32-bit timer */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
