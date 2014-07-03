/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_stm32f1
 * @{
 *
 * @file            hwtimer_cpu.h
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS   (4)             /**< the CPU implementation supports 3 HW timers */
#define HWTIMER_SPEED       (2000U)         /**< the HW timer runs with 2KHz */
#define HWTIMER_MAXTICKS    (0xFFFF)        /**< 16-bit timer */
/** @} */

#endif /* HWTIMER_CPU_H_ */
/** @} */
