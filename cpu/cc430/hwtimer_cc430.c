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

volatile uint16_t timer_round;

#define CCRA0_NUM_TO_INDEX(ccr)  ((ccr) - 1)

void timerA_init(void)
{
    volatile unsigned int *ccr = &TA0CCR0;
    volatile unsigned int *ctl = &TA0CCTL0;
    timer_round = 0;                         /* Set to round 0 */
    TA0CTL = TASSEL_1 + TACLR;               /* Clear the timer counter, set ACLK */
    TA0CTL &= ~TAIFG;                        /* Clear the IFG */
    TA0CTL &= ~(TAIE);                       /* DISABLE TAIE (overflow IRQ):
                                                see hereafter why we do this... */

    for (int i = 1; i <= HWTIMER_MAXTIMERS; i++) {
        *(ccr + i) = 0;
        *(ctl + i) &= ~(CCIFG);
        *(ctl + i) &= ~(CCIE);

        /* intialize the corresponding msp430_timer struct */
        short index = CCRA0_NUM_TO_INDEX(i);
        msp430_timer[index].base_timer = TIMER_A;
        msp430_timer[index].ccr_num = i;
        msp430_timer[index].target_round = 0;
    }

    /* we use TA0CCR0 for handling the timer_round incrementation */
    TA0CCR0 = 0x8000;    /* 32768 Hz for TimerA0... */
    TA0CCTL0 = CCIE;     /* ... so timer_round is actually a seconds counter */

    /* activate TimerA0 in "up" mode */
    TA0CTL |= MC_1;
}

interrupt(TIMER0_A0_VECTOR) __attribute__((naked)) timer0_a0_isr(void)
{
    /* we use CCR0 interrupt to manage timerA counter overflow, that is:
       to extend timerA counter to 32-bit (or more) by software;
       normally, we should use TAIV interrupt to that end... but TI had
       the (stupid) idea to give it latter the least precision of all
       timer-related interrupts, while CCR0 interrupt has the highest
       priority for TimerA; consequently, we have to use the latter to
       have a robust enough mechanism */

    __enter_isr();

    /* immediately clear Timer A counter */
    TA0CTL |= TACLR;

    /* increment timer_round */
    timer_round++;

    DEBUG("TimerA0 overflow\n");

    __exit_isr();
}

interrupt(TIMER0_A1_VECTOR) __attribute__((naked)) timer0_a1_5_isr(void)
{
    __enter_isr();

    short ccr_num = (TA0IV) >> 1;
    if (ccr_num <= TIMER_A_MAXCOMP) {
        short index = CCRA0_NUM_TO_INDEX(ccr_num);
        /* check which CCR has been hit and if the overflow counter
           for this timer has been reached (or exceeded);
           there is indeed a race condition where an hwtimer
           due to fire in the next round can be set after
           the timer's counter has overflowed but *before*
           timer_round incrementation has occured (when
           interrupts are disabled for any reason), thus
           effectively setting the timer one round in the past! */
        int16_t round_delta = msp430_timer[index].target_round - timer_round;
        /* in order to correctly handle timer_round overflow,
           we must fire the timer when, for example,
           timer_round == 0 and overflow_interrupt[timer] == 65535;
           to that end, we compute the difference between the two
           on a 16-bit signed integer: any difference >= +32768 will
           thus overload to a negative number; we should then
           correctly fire "overdue" timers whenever the case */
        if (round_delta <= 0) {
            timer_unset(index);
            int_handler(index);
        }
    }

    __exit_isr();
}
