/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2015 FreshTemp, LLC.
 * SPDX-FileCopyrightText: 2014-2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_saml21
 * @{
 *
 * @file        startup.c
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

/* SAML21 specific interrupt vector */
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

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    (void*) isr_pm,                 /*  0 Power Manager */
    (void*) isr_wdt,                /*  1 Watchdog Timer */
    (void*) isr_rtc,                /*  2 Real-Time Counter */
    (void*) isr_eic,                /*  3 External Interrupt Controller */
    (void*) isr_nvmctrl,            /*  4 Non-Volatile Memory Controller */
    (void*) isr_dmac,               /*  5 Direct Memory Access Controller */
    (void*) isr_usb,                /*  6 Universal Serial Bus */
    (void*) isr_evsys,              /*  7 Event System Interface */
    (void*) isr_sercom0,            /*  8 Serial Communication Interface 0 */
    (void*) isr_sercom1,            /*  9 Serial Communication Interface 1 */
    (void*) isr_sercom2,            /* 10 Serial Communication Interface 2 */
    (void*) isr_sercom3,            /* 11 Serial Communication Interface 3 */
    (void*) isr_sercom4,            /* 12 Serial Communication Interface 4 */
    (void*) isr_sercom5,            /* 13 Serial Communication Interface 5 */
    (void*) isr_tcc0,               /* 14 Timer Counter Control 0 */
    (void*) isr_tcc1,               /* 15 Timer Counter Control 1 */
    (void*) isr_tcc2,               /* 16 Timer Counter Control 2 */
    (void*) isr_tc0,                /* 17 Basic Timer Counter 0 */
    (void*) isr_tc1,                /* 18 Basic Timer Counter 1 */
    (void*) isr_tc2,                /* 19 Basic Timer Counter 2 */
    (void*) isr_tc3,                /* 20 Basic Timer Counter 3 */
    (void*) isr_tc4,                /* 21 Basic Timer Counter 4 */
    (void*) isr_adc,                /* 22 Analog Digital Converter */
    (void*) isr_ac,                 /* 23 Analog Comparators */
    (void*) isr_dac,                /* 24 Digital Analog Converter */
    (void*) isr_ptc,                /* 25 Peripheral Touch Controller */
    (void*) isr_aes,                /* 26 AES */
    (void*) isr_trng,               /* 27 TRNG True Random Number Generator */
};
