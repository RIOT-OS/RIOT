/**
 * Native CPU hwtimer_cpu configuration
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @{
 * @ingroup native_hwtimer
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HWTIMER_MAXTIMERS 1
#define HWTIMER_SPEED 1000000
#define HWTIMER_MAXTICKS (0xFFFFFFFF)
#define HWTIMER_SPIN_BARRIER 20

#ifdef __cplusplus
}
#endif

#endif /* HWTIMER_CPU_H_ */
