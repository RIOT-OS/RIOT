/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k20
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Finn Wilke <finn.wilke@fu-berlin.de>
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
#define HWTIMER_MAXTIMERS   1              /**< We only support one timer because hwtimer uses the LPTMR0 */
#define HWTIMER_SPEED       1000000        /**< the HW timer runs with 1Mhz */
#define HWTIMER_MAXTICKS    (0xFFFF)       /**< 16-bit timer */
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
