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

msp430_timer_t msp430_timer[HWTIMER_MAXTIMERS];

#define CCRA_NUM_TO_INDEX(ccr)   (ccr)
#define CCRB_NUM_TO_INDEX(ccr)  ((ccr) + TIMER_A_MAXCOMP)

void timerA_init(void)
{
    TACTL = TASSEL_1 + TACLR;                /* Clear the timer counter, set ACLK */
    TACTL &= ~(TAIFG);                       /* Clear the IFG */
    TACTL &= ~(TAIE);                        /* Disable TAIE (overflow IRQ) */

    for (uint8_t i = 0; i < TIMER_A_MAXCOMP; i++) {
        volatile unsigned int *ccr = &TACCR0 + (i);
        volatile unsigned int *ctl = &TACCTL0 + (i);
        *ccr = 0;
        *ctl &= ~(CCIFG);
        *ctl &= ~(CCIE);

        /* intialize the corresponding msp430_timer struct */
        short index = CCRA_NUM_TO_INDEX(i);
        msp430_timer[index].base_timer = TIMER_A;
        msp430_timer[index].ccr_num = i;
    }

    TACTL |= MC_2;
}

void timerB_init(void)
{

    TBCTL = TBSSEL_1 + TBCLR;                /* Clear the timer counter, set ACLK */
    TBCTL &= ~(TBIFG);                       /* Clear the IFG */
    TBCTL &= ~(TBIE);                        /* Disable TBIE (overflow IRQ) */

    for (uint8_t i = 0; i < TIMER_B_MAXCOMP; i++) {
        volatile unsigned int *ccr = &TBCCR0 + (i);
        volatile unsigned int *ctl = &TBCCTL0 + (i);
        *ccr = 0;
        *ctl &= ~(CCIFG);
        *ctl &= ~(CCIE);

        /* intialize the corresponding msp430_timer struct */
        short index = CCRB_NUM_TO_INDEX(i);
        msp430_timer[index].base_timer = TIMER_B;
        msp430_timer[index].ccr_num = i;
    }

    TBCTL |= MC_2;
}

interrupt(TIMERA0_VECTOR) __attribute__((naked)) timerA_isr_ccr0(void)
{
    __enter_isr();

    short timer = CCRA_NUM_TO_INDEX(0);
    timer_unset(timer);
    int_handler(timer);

    __exit_isr();
}

interrupt(TIMERA1_VECTOR) __attribute__((naked)) timerA_isr(void)
{
    __enter_isr();

    /* determine which CCR has been hit, and fire the appropriate callback */
    short timer = CCRA_NUM_TO_INDEX(TAIV >> 1);
    timer_unset(timer);
    int_handler(timer);

    __exit_isr();
}

interrupt(TIMERB0_VECTOR) __attribute__((naked)) timerB_isr_ccr0(void)
{
    __enter_isr();

    short timer = CCRB_NUM_TO_INDEX(0);
    timer_unset(timer);
    int_handler(timer);

    __exit_isr();
}

interrupt(TIMERB1_VECTOR) __attribute__((naked)) timerB_isr(void)
{
    __enter_isr();

    /* determine which CCR has been hit, and fire the appropriate callback */
    short timer = CCRB_NUM_TO_INDEX(TBIV >> 1);
    timer_unset(timer);
    int_handler(timer);

    __exit_isr();
}
