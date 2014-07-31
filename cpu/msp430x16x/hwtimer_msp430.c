/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file        hwtimer_msp430_.c
 * @brief       MSP430x16xx timer functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Milan Babel <babel@inf.fu-berlin.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "hwtimer.h"
#include "arch/hwtimer_arch.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

extern void (*int_handler)(int);
extern void timer_unset(short timer);
extern volatile uint16_t overflow_interrupt[HWTIMER_MAXTIMERS+1];
extern volatile uint16_t timer_round;

void timerA_init(void)
{
    volatile unsigned int *ccr;
    volatile unsigned int *ctl;
    timer_round = 0;                         /* Set to round 0 */
    TACTL = TASSEL_1 + TACLR;                /* Clear the timer counter, set ACLK */
    TACTL &= ~TAIFG;                         /* Clear the IFG */
    TACTL |= TAIE;                           /* Enable TAIE (overflow IRQ) */

    for (int i = 0; i < HWTIMER_MAXTIMERS; i++) {
        ccr = &TACCR0 + (i);
        ctl = &TACCTL0 + (i);
        *ccr = 0;
        *ctl &= ~(CCIFG);
        *ctl &= ~(CCIE);
    }

    TACTL |= MC_2;
}

interrupt(TIMERA0_VECTOR) __attribute__((naked)) timer_isr_ccr0(void)
{
    __enter_isr();

    if (overflow_interrupt[0] == timer_round) {
        timer_unset(0);
        int_handler(0);
    }

    __exit_isr();
}

interrupt(TIMERA1_VECTOR) __attribute__((naked)) timer_isr(void)
{
    __enter_isr();

    short taiv_reg = TAIV;
    if (taiv_reg == 0x0A) {
        /* TAIV = 0x0A means overflow */
        DEBUG("Overflow\n");
        timer_round++;
    }
    else {
        short timer = taiv_reg >> 1;
        /* check which CCR has been hit and if the overflow counter
           for this timer has been reached (or exceeded);
            there is indeed a race condition where an hwtimer
            due to fire in the next round can be set after
            the timer's counter has overflowed but *before*
            timer_round incrementation has occured (when
            interrupts are disabled for any reason), thus
            effectively setting the timer one round in the past! */
        int16_t round_delta = overflow_interrupt[timer] - timer_round;
        /* in order to correctly handle timer_round overflow,
           we must fire the timer when, for example,
           timer_round == 0 and overflow_interrupt[timer] == 65535;
           to that end, we compute the difference between the two
           on a 16-bit signed integer: any difference >= +32768 will
           thus overload to a negative number; we should then
           correctly fire "overdue" timers whenever the case */
        if (round_delta <= 0) {
            timer_unset(timer);
            int_handler(timer);
        }
    }

    __exit_isr();
}
