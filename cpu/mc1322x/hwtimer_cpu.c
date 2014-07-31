/*
 * hwtimer_cpu.c - architecture dependent hardware timer functionality
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "mc1322x.h"
#include "cpu.h"
#include "arch/hwtimer_arch.h"
#include "irq.h"

#include <stdio.h>

/* High level interrupt handler */
static void (*int_handler)(int);

#define TMRx_ANY_INTERRUPT 0xa800

void tmr_isr(void) {
    /* detemine which timer caused the interrupt */
    if (TMR0->SCTRLbits.TCF && TMR0->SCTRLbits.TCFIE) {
        TMR0->SCTRLbits.TCF = 0;
        TMR0->CSCTRLbits.TCF1 = 0;
        TMR0->ENBL &= ~(1<<0);
        int_handler(0);
    }
    else if (TMR1->SCTRLbits.TCF && TMR1->SCTRLbits.TCFIE) {
        TMR1->SCTRLbits.TCF = 0;
        TMR1->CSCTRLbits.TCF1 = 0;
        TMR0->ENBL &= ~(1<<1);
        int_handler(1);
    }
    else if (TMR2->SCTRLbits.TCF && TMR2->SCTRLbits.TCFIE) {
        TMR2->SCTRLbits.TCF = 0;
        TMR2->CSCTRLbits.TCF1 = 0;
        TMR0->ENBL &= ~(1<<2);
        int_handler(2);
    }
    else if (TMR3->SCTRLbits.TCF && TMR3->SCTRLbits.TCFIE) {
        TMR3->SCTRLbits.TCF = 0;
        TMR3->CSCTRLbits.TCF1 = 0;
        TMR0->ENBL &= ~(1<<3);
        int_handler(3);
    }
}

void timer_x_init(volatile struct TMR_struct* const TMRx) {
    /* Reset the timer */

    /* Clear status */
    TMRx->SCTRL = 0;
    /* disable interrupt */
    TMRx->CSCTRL =0x0000;
    /* Reload/initialize to zero */
    TMRx->LOAD = 0;

    /* disable comparison */
    TMRx->COMP1 = 0;
    TMRx->CMPLD1 = 0;

    /* set counter to zero */
    TMRx->CNTR = TMRx->LOAD;

    /* set timer control bits */
    TMRx->CTRLbits.COUNT_MODE = 1;              /* use rising edge of primary source */
    TMRx->CTRLbits.PRIMARY_CNT_SOURCE = 0x0f;   /* Perip. clock with 128 prescale (for 24MHz = 187500Hz) */
    TMRx->CTRLbits.SECONDARY_CNT_SOURCE = 0x00; /* don't need this */
    TMRx->CTRLbits.ONCE = 0x01;                 /* don't keep counting */
    TMRx->CTRLbits.LENGTH = 0x00;               /* continue counting */
    TMRx->CTRLbits.DIR = 0x00;                  /* count up */
    TMRx->CTRLbits.CO_INIT = 0x00;              /* other counters cannot force a reinitialization of this counter*/
    TMRx->CTRLbits.OUTPUT_MODE = 0x00;          /* OFLAG is asserted while counter is active */
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu) {
    int_handler = handler;

    /* TODO: do scaling voodoo */
    (void) fcpu;

    /* disable all timers */
    TMR0->ENBL = 0;
    timer_x_init(TMR0);
    timer_x_init(TMR1);
    timer_x_init(TMR2);
    timer_x_init(TMR3);

    register_isr(INT_NUM_TMR, &tmr_isr);
    hwtimer_arch_enable_interrupt();
}

/*---------------------------------------------------------------------------*/
void hwtimer_arch_enable_interrupt(void) {
    /* this enables timer interrupts in general by using the ITC.
     * Timer specific interrupt control is given by the TMRx structs. */
    //enable_irq(INT_NUM_TMR);
    ITC->INTENABLEbits.TMR = 1;
}

/*---------------------------------------------------------------------------*/
void hwtimer_arch_disable_interrupt(void) {
    /* this disables timer interrupts in general by using the ITC.
     * Timer specific interrupt control is given by the TMRx structs. */
    //disable_irq(INT_NUM_TMR);
    ITC->INTENABLEbits.TMR = 0;
}

/*---------------------------------------------------------------------------*/
void hwtimer_arch_set(unsigned long offset, short timer) {
    /* get corresponding struct for the given ::timer parameter */
    struct TMR_struct* tmr = (void *) TMR_BASE + (timer * TMR_OFFSET);

    /* disable IRQs and save the status register */
    uint32_t cpsr = disableIRQ();

    TMR0->ENBL &= ~(1<<timer);                  /* disable timer */
    tmr->COMP1 = tmr->CNTR + offset;            /* load the current value + offset into the compare register */
    tmr->CSCTRLbits.TCF1 = 0;                   /* reset compare flag */
    tmr->CSCTRLbits.TCF1EN = 1;                 /* enable intterupts when TCF1 is set \ */
    TMR0->ENBL |= (1<<timer);                   /* enable timer */
    tmr->SCTRLbits.TCFIE = 1;                   /* enable interrupts when TCF is one  - do we need both?*/

    /* restore status register */
    restoreIRQ(cpsr);
}

/*---------------------------------------------------------------------------*/
void hwtimer_arch_set_absolute(unsigned long value, short timer) {
    /* get corresponding struct for the given ::timer parameter */
    struct TMR_struct* tmr = (void *) TMR_BASE + (timer * TMR_OFFSET);

    /* disable IRQs and save the status register */
    uint32_t cpsr = disableIRQ();

    TMR0->ENBL &= ~(1<<timer);                  /* disable timer */
    tmr->COMP1 = value;                         /* load the value into the compare register */
    tmr->CSCTRLbits.TCF1 = 0;                   /* reset compare flag */
    tmr->CSCTRLbits.TCF1EN = 1;                 /* enable interrupts when TCF1 is set \ */
    TMR0->ENBL |= (1<<timer);                   /* enable timer */
    tmr->SCTRLbits.TCFIE = 1;                   /* enable interrupts when TCF is one  - do we need both?*/

    /* restore status register */
    restoreIRQ(cpsr);
}

/*---------------------------------------------------------------------------*/
void hwtimer_arch_unset(short timer) {
    /* get corresponding struct for the given ::timer parameter */
    struct TMR_struct* tmr = (void *) TMR_BASE + (timer + TMR_OFFSET);

    tmr->CSCTRLbits.TCF1 = 0;                   /* reset compare flag */
    tmr->CSCTRLbits.TCF1EN = 0;                 /* disable interrupts for TCF1 */
    tmr->SCTRLbits.TCFIE = 0;                   /* disable interrupts for TCF */
}

/*---------------------------------------------------------------------------*/
unsigned long hwtimer_arch_now(void) {
    return TMR0->CNTR;
}
