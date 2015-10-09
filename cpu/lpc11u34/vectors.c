/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"

/* get the start of the ISR stack as defined in the linkerscript */
extern uint32_t _estack;

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* LPC11U34 specific interrupt vector */
WEAK_DEFAULT void isr_pinint0(void);
WEAK_DEFAULT void isr_pinint1(void);
WEAK_DEFAULT void isr_pinint2(void);
WEAK_DEFAULT void isr_pinint3(void);
WEAK_DEFAULT void isr_pinint4(void);
WEAK_DEFAULT void isr_pinint5(void);
WEAK_DEFAULT void isr_pinint6(void);
WEAK_DEFAULT void isr_pinint7(void);
WEAK_DEFAULT void isr_gint0(void);
WEAK_DEFAULT void isr_gint1(void);
WEAK_DEFAULT void isr_ssp1(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_ct16b0(void);
WEAK_DEFAULT void isr_ct16b1(void);
WEAK_DEFAULT void isr_ct32b0(void);
WEAK_DEFAULT void isr_ct32b1(void);
WEAK_DEFAULT void isr_ssp0(void);
WEAK_DEFAULT void isr_usart0(void);
WEAK_DEFAULT void isr_usb_irq(void);
WEAK_DEFAULT void isr_usb_fiq(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_wwdt(void);
WEAK_DEFAULT void isr_bod(void);
WEAK_DEFAULT void isr_flash(void);
WEAK_DEFAULT void isr_usb_wakeup(void);

/* interrupt vector table */
__attribute__ ((section(".vectors")))
const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M0 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* LPC specific peripheral handlers */
    (void*) isr_pinint0,            /* Pin ISR0 */
    (void*) isr_pinint1,            /* Pin ISR1 */
    (void*) isr_pinint2,            /* Pin ISR2 */
    (void*) isr_pinint3,            /* Pin ISR3 */
    (void*) isr_pinint4,            /* Pin ISR4 */
    (void*) isr_pinint5,            /* Pin ISR5 */
    (void*) isr_pinint6,            /* Pin ISR6 */
    (void*) isr_pinint7,            /* Pin ISR7 */
    (void*) isr_gint0,              /* GPIO Group ISR 0 */
    (void*) isr_gint1,              /* GPIO Group ISR 1 */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_ssp1,               /* ssp1 */
    (void*) isr_i2c0,               /* i2c0 */
    (void*) isr_ct16b0,             /* ct16b0 */
    (void*) isr_ct16b1,             /* ct16b1 */
    (void*) isr_ct32b0,             /* ct32b0 */
    (void*) isr_ct32b1,             /* ct32b1 */
    (void*) isr_ssp0,               /* ssp0 */
    (void*) isr_usart0,             /* usart0 */
    (void*) isr_usb_irq,            /* USB */
    (void*) isr_usb_fiq,            /* USB */
    (void*) isr_adc,                /* ADC */
    (void*) isr_wwdt,               /* windowed watchdog */
    (void*) isr_bod,                /* brown out */
    (void*) isr_flash,              /* brown out detect */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_usb_wakeup,         /* flash */
    (void*) (0UL),                  /* Reserved */
};
