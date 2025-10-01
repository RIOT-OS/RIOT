/*
 * SPDX-FileCopyrightText: 2018 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_saml1x
 * @{
 *
 * @file        vectors.c
 * @brief       Startup code and interrupt vector definition
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
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

/* SAML1x specific interrupt vector */
WEAK_DEFAULT void isr_system(void);
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_eic0(void);
WEAK_DEFAULT void isr_eic1(void);
WEAK_DEFAULT void isr_eic2(void);
WEAK_DEFAULT void isr_eic3(void);
WEAK_DEFAULT void isr_eic_other(void);
WEAK_DEFAULT void isr_freqm(void);
WEAK_DEFAULT void isr_nvmctrl(void);
WEAK_DEFAULT void isr_port(void);
WEAK_DEFAULT void isr_dmac0(void);
WEAK_DEFAULT void isr_dmac1(void);
WEAK_DEFAULT void isr_dmac2(void);
WEAK_DEFAULT void isr_dmac3(void);
WEAK_DEFAULT void isr_dmac_other(void);
WEAK_DEFAULT void isr_evsys0(void);
WEAK_DEFAULT void isr_evsys1(void);
WEAK_DEFAULT void isr_evsys2(void);
WEAK_DEFAULT void isr_evsys3(void);
WEAK_DEFAULT void isr_evsys_nschk(void);
WEAK_DEFAULT void isr_pac(void);
WEAK_DEFAULT void isr_sercom0_0(void);
WEAK_DEFAULT void isr_sercom0_1(void);
WEAK_DEFAULT void isr_sercom0_2(void);
WEAK_DEFAULT void isr_sercom0_other(void);
WEAK_DEFAULT void isr_sercom1_0(void);
WEAK_DEFAULT void isr_sercom1_1(void);
WEAK_DEFAULT void isr_sercom1_2(void);
WEAK_DEFAULT void isr_sercom1_other(void);
WEAK_DEFAULT void isr_sercom2_0(void);
WEAK_DEFAULT void isr_sercom2_1(void);
WEAK_DEFAULT void isr_sercom2_2(void);
WEAK_DEFAULT void isr_sercom2_other(void);
WEAK_DEFAULT void isr_tc0(void);
WEAK_DEFAULT void isr_tc1(void);
WEAK_DEFAULT void isr_tc2(void);
WEAK_DEFAULT void isr_adc_other(void);
WEAK_DEFAULT void isr_adc_resrdy(void);
WEAK_DEFAULT void isr_ac(void);
WEAK_DEFAULT void isr_dac_underrun_a(void);
WEAK_DEFAULT void isr_dac_empty(void);
WEAK_DEFAULT void isr_ptc(void);
WEAK_DEFAULT void isr_trng(void);
WEAK_DEFAULT void isr_tram(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    (void*) isr_system,             /*  0 Main Clock */
    (void*) isr_wdt,                /*  1 Watchdog Timer */
    (void*) isr_rtc,                /*  2 Real-Time Counter */
    (void*) isr_eic0,               /*  3 External Interrupt Controller */
    (void*) isr_eic1,               /*  4 External Interrupt Controller */
    (void*) isr_eic2,               /*  5 External Interrupt Controller */
    (void*) isr_eic3,               /*  6 External Interrupt Controller */
    (void*) isr_eic_other,          /*  7 External Interrupt Controller */
    (void*) isr_freqm,              /*  8 Frequency Meter */
    (void*) isr_nvmctrl,            /*  9 Non-Volatile Memory Controller */
    (void*) isr_port,               /*  10 Port Module */
    (void*) isr_dmac0,              /*  11 Direct Memory Access Controller */
    (void*) isr_dmac1,              /*  12 Direct Memory Access Controller */
    (void*) isr_dmac2,              /*  13 Direct Memory Access Controller */
    (void*) isr_dmac3,              /*  14 Direct Memory Access Controller */
    (void*) isr_dmac_other,         /*  15 Direct Memory Access Controller */
    (void*) isr_evsys0,             /*  16 Event System Interface */
    (void*) isr_evsys1,             /*  17 Event System Interface */
    (void*) isr_evsys2,             /*  18 Event System Interface */
    (void*) isr_evsys3,             /*  19 Event System Interface */
    (void*) isr_evsys_nschk,        /*  20 Event System Interface */
    (void*) isr_pac,                /*  21 Peripheral Access Controller */
    (void*) isr_sercom0_0,          /*  22 Serial Communication Interface */
    (void*) isr_sercom0_1,          /*  23 Serial Communication Interface */
    (void*) isr_sercom0_2,          /*  24 Serial Communication Interface */
    (void*) isr_sercom0_other,      /*  25 Serial Communication Interface */
    (void*) isr_sercom1_0,          /*  26 Serial Communication Interface */
    (void*) isr_sercom1_1,          /*  27 Serial Communication Interface */
    (void*) isr_sercom1_2,          /*  28 Serial Communication Interface */
    (void*) isr_sercom1_other,      /*  29 Serial Communication Interface */
    (void*) isr_sercom2_0,          /*  30 Serial Communication Interface */
    (void*) isr_sercom2_1,          /*  31 Serial Communication Interface */
    (void*) isr_sercom2_2,          /*  32 Serial Communication Interface */
    (void*) isr_sercom2_other,      /*  33 Serial Communication Interface */
    (void*) isr_tc0,                /*  34 Basic Timer Counter  */
    (void*) isr_tc1,                /*  35 Basic Timer Counter  */
    (void*) isr_tc2,                /*  36 Basic Timer Counter  */
    (void*) isr_adc_other,          /*  37 Analog Digital Converter */
    (void*) isr_adc_resrdy,         /*  38 Analog Digital Converter */
    (void*) isr_ac,                 /*  39 Analog Comparators */
    (void*) isr_dac_underrun_a,     /*  40 Digital Analog Converter */
    (void*) isr_dac_empty,          /*  41 Analog Digital Converter */
    (void*) isr_ptc,                /*  42 Peripheral Touch Controller */
    (void*) isr_trng,               /*  43 True Random Number Generator */
    (void*) isr_tram,               /*  44 Trust RAM */
};
