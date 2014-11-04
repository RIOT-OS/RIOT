/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_samd21
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS   2               /**< the CPU implementation supports 2 HW timers */
#define HWTIMER_SPEED       (124999U)       /**< the HW timer runs with 125kHz */
#define HWTIMER_MAXTICKS    (0xFFFFFFFF)        /**< 16-bit timer */
/** @} */


#endif /* __HWTIMER_CPU_H */
/** @} */
