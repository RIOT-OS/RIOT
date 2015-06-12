/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam3s
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 */

#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS   (1)             /**< the CPU implementation supports one HW timer */
#define HWTIMER_SPEED       1000000         /**< the HW timer runs with 1MHz */
#define HWTIMER_MAXTICKS    (0xFFFFFFFF)    /**< 32-bit timer */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HWTIMER_CPU_H_ */
/** @} */
