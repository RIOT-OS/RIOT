/*
 * SPDX-FileCopyrightText: 2014-2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf51
 * @{
 *
 * @file
 * @brief       Interrupt vector definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {
    dummy_handler_default();
}

void pre_startup(void)
{
    /* make sure all RAM blocks are turned on
     * -> see NRF51822 Product Anomaly Notice (PAN) #16 for more details */
    NRF_POWER->RAMON = 0xf;
}

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* nRF51822 specific interrupt vectors */
WEAK_DEFAULT void isr_power_clock(void);
WEAK_DEFAULT void isr_radio(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_spi0_twi0(void);
WEAK_DEFAULT void isr_spi1_twi1(void);
WEAK_DEFAULT void isr_gpiote(void);
WEAK_DEFAULT void isr_adc(void);
WEAK_DEFAULT void isr_timer0(void);
WEAK_DEFAULT void isr_timer1(void);
WEAK_DEFAULT void isr_timer2(void);
WEAK_DEFAULT void isr_rtc0(void);
WEAK_DEFAULT void isr_temp(void);
WEAK_DEFAULT void isr_rng(void);
WEAK_DEFAULT void isr_ecb(void);
WEAK_DEFAULT void isr_ccm_aar(void);
WEAK_DEFAULT void isr_wdt(void);
WEAK_DEFAULT void isr_rtc1(void);
WEAK_DEFAULT void isr_qdec(void);
WEAK_DEFAULT void isr_lpcomp(void);
WEAK_DEFAULT void isr_swi0(void);
WEAK_DEFAULT void isr_swi1(void);
WEAK_DEFAULT void isr_swi2(void);
WEAK_DEFAULT void isr_swi3(void);
WEAK_DEFAULT void isr_swi4(void);
WEAK_DEFAULT void isr_swi5(void);

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {
    [ 0] = isr_power_clock,        /* power_clock */
    [ 1] = isr_radio,              /* radio */
    [ 2] = isr_uart0,              /* uart0 */
    [ 3] = isr_spi0_twi0,          /* spi0_twi0 */
    [ 4] = isr_spi1_twi1,          /* spi1_twi1 */
    [ 6] = isr_gpiote,             /* gpiote */
    [ 7] = isr_adc,                /* adc */
    [ 8] = isr_timer0,             /* timer0 */
    [ 9] = isr_timer1,             /* timer1 */
    [10] = isr_timer2,             /* timer2 */
    [11] = isr_rtc0,               /* rtc0 */
    [12] = isr_temp,               /* temp */
    [13] = isr_rng,                /* rng */
    [14] = isr_ecb,                /* ecb */
    [15] = isr_ccm_aar,            /* ccm_aar */
    [16] = isr_wdt,                /* wdt */
    [17] = isr_rtc1,               /* rtc1 */
    [18] = isr_qdec,               /* qdec */
    [19] = isr_lpcomp,             /* lpcomp */
    [20] = isr_swi0,               /* swi0 */
    [21] = isr_swi1,               /* swi1 */
    [22] = isr_swi2,               /* swi2 */
    [23] = isr_swi3,               /* swi3 */
    [24] = isr_swi4,               /* swi4 */
    [25] = isr_swi5,               /* swi5 */
};
