/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#include <stdint.h>

#include "cpu.h"

#ifdef __MSP430_HAS_TA2__
#define HWTIMER_MAXTIMERS 2
#endif
#ifdef __MSP430_HAS_TA3__
#define HWTIMER_MAXTIMERS 3
#endif
#ifdef __MSP430_HAS_T0A5__
#define HWTIMER_MAXTIMERS 5
#endif


#ifndef HWTIMER_MAXTIMERS
#warning "HWTIMER_MAXTIMERS UNSET!"
#define HWTIMER_MAXTIMERS 0
#endif

#define HWTIMER_SPEED  (F_RC_OSCILLATOR)
#define HWTIMER_MAXTICKS (0xFFFFFFFF)

#endif // __HWTIMER_CPU_H
