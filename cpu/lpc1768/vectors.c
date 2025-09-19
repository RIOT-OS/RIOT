/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    isr_wdt,                /* watchdog timer */
    isr_timer0,             /* timer0 */
    isr_timer1,             /* timer1 */
    isr_timer2,             /* timer2 */
    isr_timer3,             /* timer3 */
    isr_uart0,              /* uart0 */
    isr_uart1,              /* uart1 */
    isr_uart2,              /* uart2 */
    isr_uart3,              /* uart3 */
    isr_pwm1,               /* pwm1 */
    isr_i2c0,               /* i2c0 */
    isr_i2c1,               /* i2c1 */
    isr_i2c2,               /* i2c2 */
    isr_spi,                /* spi */
    isr_ssp0,               /* ssp0 */
    isr_ssp1,               /* ssp1 */
    isr_pll0,               /* pll0 (main pll) */
    isr_rtc,                /* real time clock */
    isr_eint0,              /* external interrupt 0 */
    isr_eint1,              /* external interrupt 1 */
    isr_eint2,              /* external interrupt 2 */
    isr_eint3,              /* external interrupt 3 */
    isr_adc,                /* a/d converter */
    isr_bod,                /* brown out detect */
    isr_usb,                /* usb */
    isr_can,                /* can */
    isr_dma,                /* gp dma */
    isr_i2s,                /* i2s */
    isr_enet,               /* ethernet */
    isr_rit,                /* repetitive interrupt timer */
    isr_mcpwm,              /* motor control pwm */
    isr_qei,                /* quadrature encoder interface */
    isr_pll1,               /* pll1 (usb pll) */
};
