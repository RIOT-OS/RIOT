/**
 * Native CPU hwtimer_cpu configuration
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @{
 * @ingroup native_hwtimer
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

/* TODO: choose more appropriate values here? */
#define ARCH_MAXTIMERS 4
#define HWTIMER_SPEED 1000000
#define HWTIMER_MAXTICKS (0xFFFFFFFF)

#endif /* HWTIMER_CPU_H_ */
