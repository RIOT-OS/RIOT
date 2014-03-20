/*
 * Copyright (C) 2014 INRIA
 *
 * The source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
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
#include "hwtimer_arch.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static uint32_t ticks = 0;

extern void (*int_handler)(int);
extern void timer_unset(short timer);
extern uint16_t overflow_interrupt[HWTIMER:_MAXTIMERS+1];
extern uint16_t timer_round;

void timerA_init(void)
{
    volatile unsigned int *ccr = &TA0CCR0;
    volatile unsigned int *ctl = &TA0CCTL0;
    ticks = 0;                               /* Set tick counter value to 0 */
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
    __exit_isr();
}

interrupt(TIMER0_A1_VECTOR) __attribute__((naked)) timer0_a1_5_isr(void)
{
    __enter_isr();

    short taiv = TA0IV;
    short timer = taiv / 2;
    /* TAIV = 0x0E means overflow */
    if (taiv == 0x0E) {
        DEBUG("Overflow\n");
        timer_round += 1;
    }
    /* check which CCR has been hit and if the overflow counter for this timer
     * has been reached */
    else if (overflow_interrupt[timer] == timer_round) {
        timer_unset(timer);
        int_handler(timer);
    }

    __exit_isr();
}
