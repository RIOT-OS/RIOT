/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cc430
 * @{
 */

/**
 * @file
 * @brief       cc430 hardware timer driver
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Aleksandr Mikoff <sir.enmity@gmail.com>
 *
 */

#include "cpu.h"
#include "hwtimer.h"
#include "arch/hwtimer_arch.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


extern void (*int_handler)(int);
extern void timer_unset(short timer);

msp430_timer_t msp430_timer[HWTIMER_MAXTIMERS];

void timerA_init(void)
{
    volatile unsigned int *ccr = &TA0CCR0;
    volatile unsigned int *ctl = &TA0CCTL0;

    TA0CTL = TASSEL_1 + TACLR;               /* Clear the timer counter, set ACLK */
    TA0CTL &= ~TAIFG;                        /* Clear the IFG */
    TA0CTL &= ~TAIE;                         /* Disable TAIE (overflow IRQ) */

    for (int i = 0; i < HWTIMER_MAXTIMERS; i++) {
        *(ccr + i) = 0;
        *(ctl + i) &= ~(CCIFG);
        *(ctl + i) &= ~(CCIE);
    }

    TA0CTL |= MC_2;
}

interrupt(TIMER0_A0_VECTOR) __attribute__((naked)) timer0_a0_isr(void)
{
    __enter_isr();

    timer_unset(0);
    int_handler(0);

    __exit_isr();
}

interrupt(TIMER0_A1_VECTOR) __attribute__((naked)) timer0_a1_5_isr(void)
{
    __enter_isr();

    /* determine which CCR has been hit, and fire the appropriate callback */
    short timer = TA0IV >> 1;
    timer_unset(timer);
    int_handler(timer);

    __exit_isr();
}
