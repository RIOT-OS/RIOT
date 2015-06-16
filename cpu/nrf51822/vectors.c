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

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M0 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* nRF51 specific peripheral handlers */
    (void*) isr_power_clock,        /* power_clock */
    (void*) isr_radio,              /* radio */
    (void*) isr_uart0,              /* uart0 */
    (void*) isr_spi0_twi0,          /* spi0_twi0 */
    (void*) isr_spi1_twi1,          /* spi1_twi1 */
    (void*) (0UL),                  /* reserved */
    (void*) isr_gpiote,             /* gpiote */
    (void*) isr_adc,                /* adc */
    (void*) isr_timer0,             /* timer0 */
    (void*) isr_timer1,             /* timer1 */
    (void*) isr_timer2,             /* timer2 */
    (void*) isr_rtc0,               /* rtc0 */
    (void*) isr_temp,               /* temp */
    (void*) isr_rng,                /* rng */
    (void*) isr_ecb,                /* ecb */
    (void*) isr_ccm_aar,            /* ccm_aar */
    (void*) isr_wdt,                /* wdt */
    (void*) isr_rtc1,               /* rtc1 */
    (void*) isr_qdec,               /* qdec */
    (void*) isr_lpcomp,             /* lpcomp */
    (void*) isr_swi0,               /* swi0 */
    (void*) isr_swi1,               /* swi1 */
    (void*) isr_swi2,               /* swi2 */
    (void*) isr_swi3,               /* swi3 */
    (void*) isr_swi4,               /* swi4 */
    (void*) isr_swi5,               /* swi5 */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
    (void*) (0UL),                  /* reserved */
};
