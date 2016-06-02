/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu_conf.h"
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
/* SAMR21 specific interrupt vector */
WEAK_DEFAULT void isr_pm(void);
WEAK_DEFAULT void isr_sysctrl(void);
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
WEAK_DEFAULT void isr_tc3(void);
WEAK_DEFAULT void isr_tc4(void);
WEAK_DEFAULT void isr_tc5(void);
WEAK_DEFAULT void isr_tc6(void);
WEAK_DEFAULT void isr_tc7(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_ac(void);
WEAK_DEFAULT void isr_dac(void);
WEAK_DEFAULT void isr_ptc(void);
WEAK_DEFAULT void isr_i2c(void);

/* interrupt vector table */
ISR_VECTORS struct {
    void* _estack;
    void(*vectors[CPU_IRQ_NUMOF + CPU_NONISR_EXCEPTIONS])(void);
} interrupt_vector = {
    /* Exception stack pointer */
    &_estack,                       /* pointer to the top of the stack */
    {
        /* Cortex-M0+ handlers */
        reset_handler_default,  /* entry point of the program */
        nmi_default,            /* non maskable interrupt handler */
        hard_fault_default,     /* hard fault exception */
        (0UL),                  /* reserved */
        (0UL),                  /* reserved */
        (0UL),                  /* reserved */
        (0UL),                  /* reserved */
        (0UL),                  /* reserved */
        (0UL),                  /* reserved */
        (0UL),                  /* reserved */
        isr_svc,                /* system call interrupt, in RIOT used for
                                 * switching into thread context on boot */
        (0UL),                  /* reserved */
        (0UL),                  /* reserved */
        isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                 * context switching is happening here */
        isr_systick,            /* SysTick interrupt, not used in RIOT */
        /* Atmel specific peripheral handlers */
        isr_pm,                 /*  0 Power Manager */
        isr_sysctrl,            /*  1 System Control */
        isr_wdt,                /*  2 Watchdog Timer */
        isr_rtc,                /*  3 Real-Time Counter */
        isr_eic,                /*  4 External Interrupt Controller */
        isr_nvmctrl,            /*  5 Non-Volatile Memory Controller */
        isr_dmac,               /*  6 Direct Memory Access Controller */
        isr_usb,                /*  7 Universal Serial Bus */
        isr_evsys,              /*  8 Event System Interface */
        isr_sercom0,            /*  9 Serial Communication Interface 0 */
        isr_sercom1,            /* 10 Serial Communication Interface 1 */
        isr_sercom2,            /* 11 Serial Communication Interface 2 */
        isr_sercom3,            /* 12 Serial Communication Interface 3 */
        isr_sercom4,            /* 13 Serial Communication Interface 4 */
        isr_sercom5,            /* 14 Serial Communication Interface 5 */
        isr_tcc0,               /* 15 Timer Counter Control 0 */
        isr_tcc1,               /* 16 Timer Counter Control 1 */
        isr_tcc2,               /* 17 Timer Counter Control 2 */
        isr_tc3,                /* 18 Basic Timer Counter 0 */
        isr_tc4,                /* 19 Basic Timer Counter 1 */
        isr_tc5,                /* 20 Basic Timer Counter 2 */
        isr_tc6,                /* 21 Basic Timer Counter 3 */
        isr_tc7,                /* 22 Basic Timer Counter 4 */
        isr_adc,                /* 23 Analog Digital Converter */
        isr_ac,                 /* 24 Analog Comparators */
        isr_dac,                /* 25 Digital Analog Converter */
        isr_ptc,                /* 26 Peripheral Touch Controller */
        isr_i2c                 /* 27 Inter-IC Sound Interface */
    }
};
