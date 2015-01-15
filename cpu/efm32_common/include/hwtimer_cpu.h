/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_efm32gg
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
#define HWTIMER_MAXTIMERS   1               /**< the CPU implementation supports 4 HW timers. Only one currently enabled */
#define HWTIMER_SPEED       1000000         /**< the HW timer runs at 750kHz. Currently lying about this for simplicity */
#define HWTIMER_MAXTICKS    (0xFFFFFFFF)    /**< 32-bit timer */

/** @} */

#endif /* __HWTIMER_CPU_H */
/** @} */
//Test