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

volatile uint16_t overflow_interrupt[HWTIMER_MAXTIMERS];
volatile uint16_t timer_round;

#define CCRA0_NUM_TO_INDEX(ccr)  ((ccr) - 1)

void timerA_init(void)
{
    volatile unsigned int *ccr = &TA0CCR0;
    volatile unsigned int *ctl = &TA0CCTL0;
    timer_round = 0;                         /* Set to round 0 */
    TA0CTL = TASSEL_1 + TACLR;               /* Clear the timer counter, set ACLK */
    TA0CTL &= ~TAIFG;                        /* Clear the IFG */
    TA0CTL |= TAIE;                          /* Enable TAIE (overflow IRQ) */

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

    if (overflow_interrupt[0] == timer_round) {
        timer_unset(0);
        int_handler(0);
    }

    DEBUG("TimerA0 overflow\n");

    __exit_isr();
}

interrupt(TIMER0_A1_VECTOR) __attribute__((naked)) timer0_a1_5_isr(void)
{
    __enter_isr();

    short taiv_reg = TA0IV;
    if (taiv_reg == 0x0E) {
        /* TAIV == 0x0E means overflow */
        timer_round++;
        DEBUG("Overflow\n");
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
