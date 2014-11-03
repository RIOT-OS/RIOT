/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc2538
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Ian Martin <ian@locicontrols.com>
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

#define OSC32K_CAL_RATIO    977                               /**< CC2538 low-frequency clock calibration ratio = round(32000000 / 32768) */

#define HWTIMER_MAXTIMERS   1                                 /**< Number of hwtimers */

#if SYS_CTRL_OSC32K_USE_XTAL
    #define HWTIMER_SPEED   XOSC32K_FREQ                      /**< The hardware timer runs at 32.768 kHz */
#else
    #define HWTIMER_SPEED   (XOSC32M_FREQ / OSC32K_CAL_RATIO) /**< The hardware timer runs at ~32.753 kHz */
#endif

#define HWTIMER_MAXTICKS    0xffffffff                        /**< 32-bit timer */

/*
 * When setting a new compare value, the value must be
 * at least 5 more than the current sleep timer value.
 * Otherwise, the timer compare event may be lost.
 */
#define HWTIMER_SPIN_BARRIER 5

/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
