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
 * @brief       MSP430Fxyz timer functions
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

void timerA_init(void)
{
    volatile unsigned int *ccr;
    volatile unsigned int *ctl;

    TACTL = TASSEL_1 + TACLR;                /* Clear the timer counter, set ACLK */
    TACTL &= ~TAIFG;                         /* Clear the IFG */
    TACTL &= ~TAIE;                          /* Disable TAIE (overflow IRQ) */

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

    timer_unset(0);
    int_handler(0);

    __exit_isr();
}

interrupt(TIMERA1_VECTOR) __attribute__((naked)) timer_isr(void)
{
    __enter_isr();

    /* determine which CCR has been hit, and fire the appropriate callback */
    short timer = TAIV >> 1;
    timer_unset(timer);
    int_handler(timer);

    __exit_isr();
}
