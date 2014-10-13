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

#include "gptimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS   NUM_CHANNELS_PER_GPTIMER /**< Number of hwtimers */
#define HWTIMER_SPEED       1000000         /**< The hardware timer runs at 1MHz */
#define HWTIMER_MAXTICKS    0xFFFFFFFF      /**< 32-bit timer */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* __HWTIMER_CPU_H */
/** @} */
