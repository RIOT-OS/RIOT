/******************************************************************************
Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file is subject to the terms and conditions of the LGPLv2.
See the file LICENSE in the top level directory for more details.
*******************************************************************************/

#ifndef __HWTIMER_CPU_H
#define __HWTIMER_CPU_H

#include <stdint.h>

#include "cpu.h"

#if defined (__MSP430_HAS_TA2__)
#define TIMER_A_MAXCOMP  1
#elif defined (__MSP430_HAS_TA3__)
#define TIMER_A_MAXCOMP  2
#elif defined (__MSP430_HAS_T0A5__)
#define TIMER_A_MAXCOMP  4
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
#define HWTIMER_MAXTICKS  (0xFFFF7FFF)

#endif // __HWTIMER_CPU_H
