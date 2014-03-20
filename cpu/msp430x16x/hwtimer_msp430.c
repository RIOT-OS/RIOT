/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
#include "hwtimer_arch.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static uint32_t ticks = 0;

extern void (*int_handler)(int);
extern void timer_unset(short timer);
extern uint16_t overflow_interrupt[HWTIMER_MAXTIMERS+1];
extern uint16_t timer_round;

void timerA_init(void)
{
    volatile unsigned int *ccr;
    volatile unsigned int *ctl;
    ticks = 0;                               /* Set tick counter value to 0 */
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

    short taiv = TAIV;
    short timer = taiv / 2;
    /* TAIV = 0x0A means overflow */
    if (taiv == 0x0A) {
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
