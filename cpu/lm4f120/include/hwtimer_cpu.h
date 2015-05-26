/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_lm4f120
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Rakendra Thapa <rakendrathapa@gmail.com>
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
#define HWTIMER_MAXTIMERS   1               /**< the CPU implementation supports 4 HW timers */
#define HWTIMER_SPEED       1000000         /**< the HW timer runs with 1MHz */
#define HWTIMER_MAXTICKS    (0xFFFF)		/**< 16-bit timer */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
