/*
 * SPDX-FileCopyrightText: 2014-2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam3
 * @{
 *
 * @file
 * @brief       Startup code and interrupt vector definition
 *
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

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* SAM3X8E specific interrupt vector */
WEAK_DEFAULT void isr_supc(void);
WEAK_DEFAULT void isr_rstc(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_rtt(void);
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_pmc(void);
WEAK_DEFAULT void isr_efc0(void);
WEAK_DEFAULT void isr_efc1(void);
WEAK_DEFAULT void isr_uart(void);
WEAK_DEFAULT void isr_smc(void);
WEAK_DEFAULT void isr_pioa(void);
WEAK_DEFAULT void isr_piob(void);
WEAK_DEFAULT void isr_pioc(void);
WEAK_DEFAULT void isr_piod(void);
WEAK_DEFAULT void isr_usart0(void);
WEAK_DEFAULT void isr_usart1(void);
WEAK_DEFAULT void isr_usart2(void);
WEAK_DEFAULT void isr_usart3(void);
WEAK_DEFAULT void isr_hsmci(void);
WEAK_DEFAULT void isr_twi0(void);
WEAK_DEFAULT void isr_twi1(void);
WEAK_DEFAULT void isr_spi0(void);
WEAK_DEFAULT void isr_ssc(void);
WEAK_DEFAULT void isr_tc0(void);
WEAK_DEFAULT void isr_tc1(void);
WEAK_DEFAULT void isr_tc2(void);
WEAK_DEFAULT void isr_tc3(void);
WEAK_DEFAULT void isr_tc4(void);
WEAK_DEFAULT void isr_tc5(void);
WEAK_DEFAULT void isr_tc6(void);
WEAK_DEFAULT void isr_tc7(void);
WEAK_DEFAULT void isr_tc8(void);
WEAK_DEFAULT void isr_pwm(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_dacc(void);
WEAK_DEFAULT void isr_dmac(void);
WEAK_DEFAULT void isr_uotghs(void);
WEAK_DEFAULT void isr_trng(void);
WEAK_DEFAULT void isr_emac(void);
WEAK_DEFAULT void isr_can0(void);
WEAK_DEFAULT void isr_can1(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_supc,               /*  0 supply controller */
    isr_rstc,               /*  1 reset controller */
    isr_rtc,                /*  2 real time clock */
    isr_rtt,                /*  3 real timer timer */
    isr_wdt,                /*  4 watchdog timer */
    isr_pmc,                /*  5 power management controller */
    isr_efc0,               /*  6 enhanced flash controller 0 */
    isr_efc1,               /*  7 enhanced flash controller 1 */
    isr_uart,               /*  8 universal asynchronous receiver transceiver */
    isr_smc,                /*  9 static memory controller */
    (0UL),
    isr_pioa,               /* 11 GPIO port A */
    isr_piob,               /* 12 GPIO port B */
    isr_pioc,               /* 13 GPIO port C */
    isr_piod,               /* 14 GPIO port D */
    (0UL),
    (0UL),
    isr_usart0,             /* 17 USART0 */
    isr_usart1,             /* 18 USART1 */
    isr_usart2,             /* 19 USART2 */
    isr_usart3,             /* 20 USART3 */
    isr_hsmci,              /* 21 multimedia card interface */
    isr_twi0,               /* 22 two-wire interface 0 */
    isr_twi1,               /* 23 two-wire interface 1 */
    isr_spi0,               /* 24 serial peripheral interface */
    (0UL),
    isr_ssc,                /* 26 synchronous serial controller */
    isr_tc0,                /* 27 timer counter 0 */
    isr_tc1,                /* 28 timer counter 1 */
    isr_tc2,                /* 29 timer counter 2 */
    isr_tc3,                /* 30 timer counter 3 */
    isr_tc4,                /* 31 timer counter 4 */
    isr_tc5,                /* 32 timer counter 5 */
    isr_tc6,                /* 33 timer counter 6 */
    isr_tc7,                /* 34 timer counter 7 */
    isr_tc8,                /* 35 timer counter 8 */
    isr_pwm,                /* 36 pulse width modulation controller */
    isr_adc,                /* 37 ADC controller */
    isr_dacc,               /* 38 DAC controller */
    isr_dmac,               /* 39 DMA controller */
    isr_uotghs,             /* 40 USB OTG high speed */
    isr_trng,               /* 41 true random number generator */
    isr_emac,               /* 42 Ethernet MAC*/
    isr_can0,               /* 43 CAN controller 0*/
    isr_can1,               /* 44 CAN controller 1*/
};
