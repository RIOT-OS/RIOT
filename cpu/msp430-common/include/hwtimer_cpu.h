/*
 * Copyright (C) 2014 Freie Universitaet Berlin (FUB) and INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup cpu
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

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#include <stdint.h>

#include "cpu.h"

#if defined (__MSP430_HAS_TA2__)
#define TIMER_A_MAXCOMP  1
#elif defined (__MSP430_HAS_TA3__)
#define TIMER_A_MAXCOMP  2
#elif defined (__MSP430_HAS_T0A5__)
#define TIMER_A_MAXCOMP  5
#else
#define TIMER_A_MAXCOMP  0
#endif

#define HWTIMER_MAXTIMERS  (TIMER_A_MAXCOMP)

#ifndef HWTIMER_MAXTIMERS
#warning "HWTIMER_MAXTIMERS UNSET!"
#define HWTIMER_MAXTIMERS  0
#endif

typedef struct {
    enum {
        TIMER_A,
        TIMER_A0,
        TIMER_B,
    } base_timer;
    uint8_t ccr_num;
    volatile uint16_t target_round;
} msp430_timer_t;

#define HWTIMER_SPEED  (F_RC_OSCILLATOR)

#ifndef CC430
#define HWTIMER_MAXTICKS  (0xFFFF7FFF)
#else
#define HWTIMER_MAXTICKS  (0xFFFFFFFF)
#endif

#endif // __HWTIMER_CPU_H
