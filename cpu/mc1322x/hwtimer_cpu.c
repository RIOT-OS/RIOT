/*
 * hwtimer_cpu.c - architecture dependent hardware timer functionality
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include <stdint.h>
#include "mc1322x.h"
#include "hwtimer_arch.h"
#include "irq.h"

/* High level interrupt handler */
static void (*int_handler)(int);

void timer_x_init(TMR_struct* TMRx) {
    /* Reset the timer */
    TMRx->ENBL = 0;
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
    TMRx->CNTR = 0;
    
    /* set timer control bits */
    TMRx->CTRLbits.COUNT_MODE = 1;              /* use rising edge of primary source */
    TMRx->CTRLbits.PRIMARY_CNT_SOURCE = 0x0f;   /* Perip. clock with 128 prescale (for 24MHz = 187500Hz) */
    TMRx->CTRLbits.SECONDARY_CNT_SOURCE = 0x00; /* don't need this */
    TMRx->CTRLbits.ONCE = 0x00;                 /* keep counting */
    TMRx->CTRLbits.LENGTH = 0x00;               /* continue counting */
    TMRx->CTRLbits.DIR = 0x00;                  /* count up */
    TMRx->CTRLbits.CO_INIT = 0x00;              /* other counters cannot force a reinitialization of this counter*/
    TMRx->CTRLbits.OUTPUT_MODE = 0x00;          /* OFLAG is asserted while counter is active */
    
    TMRx->ENBL = 0xf;                           /* enable all the timers --- why not? */
    
    /* TODO: install ISR */
};

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu) {
    int_handler = handler;
    
    /* TODO: do scaling voodoo */
    (void) fcpu;
    
    timer_x_init(TMR0);
    timer_x_init(TMR1);
    timer_x_init(TMR2);
    timer_x_init(TMR3);
};

/*---------------------------------------------------------------------------*/
void hwtimer_arch_enable_interrupt(void) {
    /* this enables timer interrupts in general by using the ITC.
     * Timer specific interrupt control is given by the TMRx structs. */
    enable_irq(TRM);
}

/*---------------------------------------------------------------------------*/
void hwtimer_arch_disable_interrupt(void) {
    /* this disables timer interrupts in general by using the ITC.
     * Timer specific interrupt control is given by the TMRx structs. */
    disable_irq(TRM);
}

/*---------------------------------------------------------------------------*/
void hwtimer_arch_set(unsigned long offset, short timer) {
    /* get corresponding struct for the given ::timer parameter */
    TMR_struct* tmr = (void *) TMR_BASE + (timer + TMR_OFFSET);
    
    /* disable IRQs and save the status register */
    unsigned long cpsr = disableIRQ();
    
    tmr->COMP1 = tmr->CNTR + offset;            /* load the current value + offset into the compare register */
    tmr->CSCTRLbits.TCF1 = 0;                   /* reset compare flag */
    tmr->CSCTRLbits.TCF1EN = 1;                 /* enable intterupts when TCF1 is set \ */
    tmr->SCTRLbits.TCFIE = 1;                   /* enable interrupts when TCF is one  - do we need both?*/ 
    
    /* restor status register */
    restoreIRQ(cpsr);
}

/*---------------------------------------------------------------------------*/
void hwtimer_arch_set_absolute(unsigned long value, short timer) {
    /* get corresponding struct for the given ::timer parameter */
    TMR_struct* tmr = (void *) TMR_BASE + (timer + TMR_OFFSET);
    
    /* disable IRQs and save the status register */
    unsigned long cpsr = disableIRQ();
    
    tmr->COMP1 = value;                         /* load the value into the compare register */
    tmr->CSCTRLbits.TCF1 = 0;                   /* reset compare flag */
    tmr->CSCTRLbits.TCF1EN = 1;                 /* enable interrupts when TCF1 is set \ */
    tmr->SCTRLbits.TCFIE = 1;                   /* enable interrupts when TCF is one  - do we need both?*/ 
    
    /* restor status register */
    restoreIRQ(cpsr);
}

/*---------------------------------------------------------------------------*/
void hwtimer_arch_unset(short timer) {
    /* get corresponding struct for the given ::timer parameter */
    TMR_struct* tmr = (void *) TMR_BASE + (timer + TMR_OFFSET);
    
    tmr->CSCTRLbits.TCF1 = 0;                   /* reset compare flag */
    tmr->CSCTRLbits.TCF1EN = 0;                 /* disable interrupts for TCF1 */
    tmr->SCTRLbits.TCFIE = 0;                   /* disable interrupts for TCF */
}

/*---------------------------------------------------------------------------*/
unsigned long hwtimer_arch_now(void) {
    return TMR0->CNTR;
}
