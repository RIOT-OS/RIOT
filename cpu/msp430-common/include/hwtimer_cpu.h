/*
 * Copyright (C) 2014 Freie Universitaet Berlin (FUB) and INRIA. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_msp430_common
 * @{
 */

/**
 * @file
 * @brief       msp430 hardware timer driver definitions
 *
 * @author      Freie Universitaet Berlin, Computer Systems and Telematics group
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 */

#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__MSP430_HAS_TA2__)
#define TIMER_A_MAXCOMP  2
#elif defined (__MSP430_HAS_TA3__)
#define TIMER_A_MAXCOMP  3
#elif defined (__MSP430_HAS_T0A5__)
#define TIMER_A_MAXCOMP  5
#else
#define TIMER_A_MAXCOMP  0
#endif

#if defined (__MSP430_HAS_TB3__)
#define TIMER_B_MAXCOMP  3
#elif defined (__MSP430_HAS_TB7__)
#define TIMER_B_MAXCOMP  7
#else
#define TIMER_B_MAXCOMP  0
#endif

#define HWTIMER_MAXTIMERS  (TIMER_A_MAXCOMP + TIMER_B_MAXCOMP)

#ifndef HWTIMER_MAXTIMERS
#warning "HWTIMER_MAXTIMERS UNSET!"
#define HWTIMER_MAXTIMERS  0
#endif

typedef struct {
    enum {
        TIMER_A,
        TIMER_B,
    } base_timer;
    uint8_t ccr_num;
} msp430_timer_t;

#define HWTIMER_SPEED  (F_RC_OSCILLATOR)
#define HWTIMER_MAXTICKS (0x0000FFFF)

#ifdef __cplusplus
}
#endif

#endif /* HWTIMER_CPU_H_ */
