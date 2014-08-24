/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief           CPU specific hwtimer configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

#include "cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Hardware timer configuration
 * @{
 */
#define HWTIMER_MAXTIMERS   1               /**< the HW timer is using the LPTMR as its hardware timer */
#define HWTIMER_SPEED       32768           /**< LPTMR is running at 32.768 kHz */
#define HWTIMER_MAXTICKS (0xFFFFFFFF) /**< Virtually extended to 32 bits from 16 bits hardware counter. */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HWTIMER_CPU_H_ */
/** @} */
