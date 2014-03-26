/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         stm32f303vc
 * @{
 * 
 * @file            hwtimer_cpu.h
 * @brief           CPU specific hwtimer configuration options
 * 
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

/**
 * @brief Hardware timer configuration
 */
#define HWTIMER_MAXTIMERS   4               ///< the CPU implementation supports 4 HW timers
#define HWTIMER_SPEED       1000000         ///< the HW timer runs with 1MHz
#define HWTIMER_MAXTICKS    (0xFFFFFFFF)    ///< 32-bit timer


#endif /* __HWTIMER_CPU_H */
/** @} */
