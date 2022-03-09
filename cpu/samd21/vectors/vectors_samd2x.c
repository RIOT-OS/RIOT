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
#include "vectors_cortexm.h"
#include "cpu.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

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
WEAK_DEFAULT void isr_tcc3(void);
WEAK_DEFAULT void isr_tc0(void);
WEAK_DEFAULT void isr_tc1(void);
WEAK_DEFAULT void isr_tc2(void);
WEAK_DEFAULT void isr_tc3(void);
WEAK_DEFAULT void isr_tc4(void);
WEAK_DEFAULT void isr_tc5(void);
WEAK_DEFAULT void isr_tc6(void);
WEAK_DEFAULT void isr_tc7(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_ac(void);
WEAK_DEFAULT void isr_ac1(void);
WEAK_DEFAULT void isr_dac(void);
WEAK_DEFAULT void isr_ptc(void);
WEAK_DEFAULT void isr_i2s(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_pm,                 /*  0 Power Manager */
    isr_sysctrl,            /*  1 System Control */
    isr_wdt,                /*  2 Watchdog Timer */
    isr_rtc,                /*  3 Real-Time Counter */
    isr_eic,                /*  4 External Interrupt Controller */
    isr_nvmctrl,            /*  5 Non-Volatile Memory Controller */
#ifdef REV_DMAC
    isr_dmac,               /*  6 Direct Memory Access Controller */
#endif
#ifdef REV_USB
    isr_usb,                /*  7 Universal Serial Bus */
#endif
    isr_evsys,              /*  8 Event System Interface */
    isr_sercom0,            /*  9 Serial Communication Interface 0 */
    isr_sercom1,            /* 10 Serial Communication Interface 1 */
    isr_sercom2,            /* 11 Serial Communication Interface 2 */
    isr_sercom3,            /* 12 Serial Communication Interface 3 */
    isr_sercom4,            /* 13 Serial Communication Interface 4 */
    isr_sercom5,            /* 14 Serial Communication Interface 5 */
#ifdef TCC0
    isr_tcc0,               /* 15 Timer Counter Control 0 */
#else
    isr_tc0,                /* 15 Basic Timer Counter 0 */
#endif
#ifdef TCC1
    isr_tcc1,               /* 16 Timer Counter Control 1 */
#else
    isr_tc1,                /* 16 Basic Timer Counter 1 */
#endif
#ifdef TCC2
    isr_tcc2,               /* 17 Timer Counter Control 2 */
#else
    isr_tc2,                /* 17 Basic Timer Counter 2 */
#endif
    isr_tc3,                /* 18 Basic Timer Counter 3 */
    isr_tc4,                /* 19 Basic Timer Counter 4 */
    isr_tc5,                /* 20 Basic Timer Counter 5 */
    isr_tc6,                /* 21 Basic Timer Counter 6 */
    isr_tc7,                /* 22 Basic Timer Counter 7 */
    isr_adc,                /* 23 Analog Digital Converter */
    isr_ac,                 /* 24 Analog Comparators */
    isr_dac,                /* 25 Digital Analog Converter */
    isr_ptc,                /* 26 Peripheral Touch Controller */
#ifdef REV_I2S
    isr_i2s,                /* 27 Inter-IC Sound Interface */
#endif
#if CPU_IRQ_NUMOF > 27
    isr_ac1,                /* 28 Analog Comparators 1 */
#endif
#if CPU_IRQ_NUMOF > 28
    isr_tcc3,               /* 29 Timer Counter Control 3 */
#endif
};
