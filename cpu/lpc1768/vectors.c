/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
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

/* LPC1768 specific interrupt vector */
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_timer2(void);
WEAK_DEFAULT void isr_timer3(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_uart1(void);
WEAK_DEFAULT void isr_uart2(void);
WEAK_DEFAULT void isr_uart3(void);
WEAK_DEFAULT void isr_pwm1(void);
WEAK_DEFAULT void isr_i2c0(void);
WEAK_DEFAULT void isr_i2c1(void);
WEAK_DEFAULT void isr_i2c2(void);
WEAK_DEFAULT void isr_spi(void);
WEAK_DEFAULT void isr_ssp0(void);
WEAK_DEFAULT void isr_ssp1(void);
WEAK_DEFAULT void isr_pll0(void);
WEAK_DEFAULT void isr_rtc(void);
WEAK_DEFAULT void isr_eint0(void);
WEAK_DEFAULT void isr_eint1(void);
WEAK_DEFAULT void isr_eint2(void);
WEAK_DEFAULT void isr_eint3(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_bod(void);
WEAK_DEFAULT void isr_usb(void);
WEAK_DEFAULT void isr_can(void);
WEAK_DEFAULT void isr_dma(void);
WEAK_DEFAULT void isr_i2s(void);
WEAK_DEFAULT void isr_enet(void);
WEAK_DEFAULT void isr_rit(void);
WEAK_DEFAULT void isr_mcpwm(void);
WEAK_DEFAULT void isr_qei(void);
WEAK_DEFAULT void isr_pll1(void);

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* LPC specific peripheral handlers */
    (void*) isr_wdt,                /* watchdog timer */
    (void*) isr_timer0,             /* timer0 */
    (void*) isr_timer1,             /* timer1 */
    (void*) isr_timer2,             /* timer2 */
    (void*) isr_timer3,             /* timer3 */
    (void*) isr_uart0,              /* uart0 */
    (void*) isr_uart1,              /* uart1 */
    (void*) isr_uart2,              /* uart2 */
    (void*) isr_uart3,              /* uart3 */
    (void*) isr_pwm1,               /* pwm1 */
    (void*) isr_i2c0,               /* i2c0 */
    (void*) isr_i2c1,               /* i2c1 */
    (void*) isr_i2c2,               /* i2c2 */
    (void*) isr_spi,                /* spi */
    (void*) isr_ssp0,               /* ssp0 */
    (void*) isr_ssp1,               /* ssp1 */
    (void*) isr_pll0,               /* pll0 (main pll) */
    (void*) isr_rtc,                /* real time clock */
    (void*) isr_eint0,              /* external interrupt 0 */
    (void*) isr_eint1,              /* external interrupt 1 */
    (void*) isr_eint2,              /* external interrupt 2 */
    (void*) isr_eint3,              /* external interrupt 3 */
    (void*) isr_adc,                /* a/d converter */
    (void*) isr_bod,                /* brown out detect */
    (void*) isr_usb,                /* usb */
    (void*) isr_can,                /* can */
    (void*) isr_dma,                /* gp dma */
    (void*) isr_i2s,                /* i2s */
    (void*) isr_enet,               /* ethernet */
    (void*) isr_rit,                /* repetitive interrupt timer */
    (void*) isr_mcpwm,              /* motor control pwm */
    (void*) isr_qei,                /* quadrature encoder interface */
    (void*) isr_pll1,               /* pll1 (usb pll) */
};
