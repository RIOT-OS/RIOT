/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *               2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samr30
 * @{
 *
 * @file        vector.c
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Pierre Godicheau <pierre.godicheau@gmail.com>
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
/* SAMR30 specific interrupt vector */
WEAK_DEFAULT void isr_pm(void);
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_eic(void);
WEAK_DEFAULT void isr_nvmctrl(void);
WEAK_DEFAULT void isr_dmac(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_evsys(void);
WEAK_DEFAULT void isr_sercom0(void);
WEAK_DEFAULT void isr_sercom1(void);
WEAK_DEFAULT void isr_sercom2(void);
WEAK_DEFAULT void isr_sercom3(void);
WEAK_DEFAULT void isr_sercom4(void);
WEAK_DEFAULT void isr_sercom5(void);
WEAK_DEFAULT void isr_tcc0(void);
WEAK_DEFAULT void isr_tcc1(void);
WEAK_DEFAULT void isr_tcc2(void);
WEAK_DEFAULT void isr_tc0(void);
WEAK_DEFAULT void isr_tc1(void);
WEAK_DEFAULT void isr_tc2(void);
WEAK_DEFAULT void isr_tc3(void);
WEAK_DEFAULT void isr_tc4(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_ac(void);
WEAK_DEFAULT void isr_dac(void);
WEAK_DEFAULT void isr_ptc(void);
WEAK_DEFAULT void isr_aes(void);
WEAK_DEFAULT void isr_trng(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M0+ handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* Atmel SAMR30 specific peripheral handlers */
    (void*) isr_pm,             /*  0 main clock, 32k oscillators control, os*/
    (void*) isr_wdt,            /*  1 watchdog timer */
    (void*) isr_rtc,            /*  2 real-time counter */
    (void*) isr_eic,            /*  3 external interrupt controller */
    (void*) isr_nvmctrl,        /*  4 non-volatile memory controller */
    (void*) isr_dmac,           /*  5 direct memory access controller */
    (void*) isr_usb,            /*  6 universal serial bus */
    (void*) isr_evsys,          /*  7 event system interface */
    (void*) isr_sercom0,        /*  8 serial communication interface 0 */
    (void*) isr_sercom1,        /*  9 serial communication interface 1 */
    (void*) isr_sercom2,        /* 10 serial communication interface 2 */
    (void*) isr_sercom3,        /* 11 serial communication interface 3 */
    (void*) isr_sercom4,        /* 12 serial communication interface 4 */
    (void*) isr_sercom5,        /* 13 serial communication interface 5 */
    (void*) isr_tcc0,           /* 14 timer counter control 0 */
    (void*) isr_tcc1,           /* 15 timer counter control 1 */
    (void*) isr_tcc2,           /* 16 timer counter control 2 */
    (void*) isr_tc0,            /* 17 basic timer counter 0 */
    (void*) isr_tc1,            /* 18 basic timer counter 1 */
    (void*) isr_tc2,            /* 19 basic timer counter 2 */
    (void*) isr_tc3,            /* 20 basic timer counter 3 */
    (void*) isr_tc4,            /* 21 basic timer counter 4 */
    (void*) isr_adc,            /* 22 analog digital converter */
    (void*) isr_ac,             /* 23 analog comparators */
    (void*) isr_dac,            /* 24 digital-to-analog converter */
    (void*) isr_ptc,            /* 25 peripheral touch controller */
    (void*) isr_aes,            /* 26 advanced encryption standard */
    (void*) isr_trng,           /* 27 true random generator */
    (void*) (0UL)               /* 28 reserved */
};