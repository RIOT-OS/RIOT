/*
 * Copyright (C) 2025 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam4s
 * @{
 *
 * @file
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

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* SAM4S specific interrupt vector */
WEAK_DEFAULT void isr_supc(void);
WEAK_DEFAULT void isr_rstc(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_rtt(void);
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_pmc(void);
WEAK_DEFAULT void isr_eefc0(void);
WEAK_DEFAULT void isr_eefc1(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_smc(void);
WEAK_DEFAULT void isr_pioa(void);
WEAK_DEFAULT void isr_piob(void);
WEAK_DEFAULT void isr_pioc(void);
WEAK_DEFAULT void isr_usart0(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_hsmci(void);
WEAK_DEFAULT void isr_twi0(void);
WEAK_DEFAULT void isr_twi1(void);
WEAK_DEFAULT void isr_spi(void);
WEAK_DEFAULT void isr_ssc(void);
WEAK_DEFAULT void isr_tc0(void);
WEAK_DEFAULT void isr_tc1(void);
WEAK_DEFAULT void isr_tc2(void);
WEAK_DEFAULT void isr_tc3(void);
WEAK_DEFAULT void isr_tc4(void);
WEAK_DEFAULT void isr_tc5(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_dacc(void);
WEAK_DEFAULT void isr_pwm(void);
WEAK_DEFAULT void isr_crccu(void);
WEAK_DEFAULT void isr_acc(void);
WEAK_DEFAULT void isr_udp(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_supc,         /*  0 Supply Controller */
    isr_rstc,         /*  1 Reset Controller */
    isr_rtc,          /*  2 Real Time Clock */
    isr_rtt,          /*  3 Real Timer Timer */
    isr_wdt,          /*  4 Watchdog Timer */
    isr_pmc,          /*  5 Power Management Controller */
    isr_eefc0,        /*  6 Enhanced Embedded Flash Controller 0 */
    isr_eefc1,        /*  7 Enhanced Embedded Flash Controller 1 */
    isr_uart0,        /*  8 Universal Asynchronous Receiver Transceiver 0 */
    isr_uart1,        /*  9 Universal Asynchronous Receiver Transceiver 1 */
    isr_smc,         /* 10 Static Memory Controller */
    isr_pioa,        /* 11 GPIO port A */
    isr_piob,        /* 12 GPIO port B */
    isr_pioc,        /* 13 GPIO port C */
    isr_usart0,      /* 14 USART0 */
    isr_usart1,      /* 15 USART1 */
    (0UL),           /* Reserved */
    (0UL),           /* Reserved */
    isr_hsmci,       /* 18 Multimedia Card Interface */
    isr_twi0,        /* 19 Two-wire Interface 0 */
    isr_twi1,        /* 20 Two-wire Interface 1 */
    isr_spi,         /* 21 Serial Peripheral Interface */
    isr_ssc,         /* 22 Synchronous Serial Controller */
    isr_tc0,         /* 23 Timer Counter 0 */
    isr_tc1,         /* 24 Timer Counter 1 */
    isr_tc2,         /* 25 Timer Counter 2 */
    isr_tc3,         /* 26 Timer Counter 3 */
    isr_tc4,         /* 27 Timer Counter 4 */
    isr_tc5,         /* 28 Timer Counter 5 */
    isr_adc,         /* 29 Analog/Digital Converter */
    isr_dacc,        /* 30 Digital/Analog Converter Controller */
    isr_pwm,         /* 31 Pulse Width Modulation */
    isr_crccu,       /* 32 CRC Calculation Unit */
    isr_acc,         /* 33 Analog Comparator Controller */
    isr_udp,         /* 34 USB Device Port */
};
