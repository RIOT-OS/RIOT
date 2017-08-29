/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
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

/* get the start of the ISR stack as defined in the linkerscript */
extern uint32_t _estack;

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
ISR_VECTOR(1) const isr_t vector_cpu[] = {
    isr_power_clock,        /* power_clock */
    isr_radio,              /* radio */
    isr_uart0,              /* uart0 */
    isr_spi0_twi0,          /* spi0_twi0 */
    isr_spi1_twi1,          /* spi1_twi1 */
    (0UL),                  /* reserved */
    isr_gpiote,             /* gpiote */
    isr_adc,                /* adc */
    isr_timer0,             /* timer0 */
    isr_timer1,             /* timer1 */
    isr_timer2,             /* timer2 */
    isr_rtc0,               /* rtc0 */
    isr_temp,               /* temp */
    isr_rng,                /* rng */
    isr_ecb,                /* ecb */
    isr_ccm_aar,            /* ccm_aar */
    isr_wdt,                /* wdt */
    isr_rtc1,               /* rtc1 */
    isr_qdec,               /* qdec */
    isr_lpcomp,             /* lpcomp */
    isr_swi0,               /* swi0 */
    isr_swi1,               /* swi1 */
    isr_swi2,               /* swi2 */
    isr_swi3,               /* swi3 */
    isr_swi4,               /* swi4 */
    isr_swi5,               /* swi5 */
    (0UL),                  /* reserved */
    (0UL),                  /* reserved */
    (0UL),                  /* reserved */
    (0UL),                  /* reserved */
    (0UL),                  /* reserved */
    (0UL),                  /* reserved */
};
