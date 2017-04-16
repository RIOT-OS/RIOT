/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       nRF52 interrupt vector definitions
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

/* Cortex-M common interrupt vectors */
WEAK_DEFAULT void isr_svc(void);
WEAK_DEFAULT void isr_pendsv(void);
WEAK_DEFAULT void isr_systick(void);
/* nRF52 specific interrupt vectors */
WEAK_DEFAULT void isr_power_clock(void);
WEAK_DEFAULT void isr_radio(void);
WEAK_DEFAULT void isr_uart0(void);
WEAK_DEFAULT void isr_spi0_twi0(void);
WEAK_DEFAULT void isr_spi1_twi1(void);
WEAK_DEFAULT void isr_nfct(void);
WEAK_DEFAULT void isr_gpiote(void);
WEAK_DEFAULT void isr_saadc(void);
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

#ifndef SOFTDEVICE_PRESENT
WEAK_DEFAULT void isr_swi0(void);
#else
/* For unknown reasons, setting PendSV pending within
 * the softdevice ISRs leads to a crash. This workaround
 * uses swi0 as trampoline.
 */
extern void thread_arch_yield(void);
void isr_swi0(void)
{
    thread_arch_yield();
}
#endif

WEAK_DEFAULT void isr_swi1(void);
WEAK_DEFAULT void isr_swi2(void);
WEAK_DEFAULT void isr_swi3(void);
WEAK_DEFAULT void isr_swi4(void);
WEAK_DEFAULT void isr_swi5(void);
WEAK_DEFAULT void isr_timer3(void);
WEAK_DEFAULT void isr_timer4(void);
WEAK_DEFAULT void isr_pwm0(void);
WEAK_DEFAULT void isr_pdm(void);
WEAK_DEFAULT void isr_mwu(void);
WEAK_DEFAULT void isr_pwm1(void);
WEAK_DEFAULT void isr_pwm2(void);
WEAK_DEFAULT void isr_spi2(void);
WEAK_DEFAULT void isr_rtc2(void);
WEAK_DEFAULT void isr_i2s(void);

#ifdef CPU_MODEL_NRF52840XXAA
WEAK_DEFAULT void isr_fpu(void);
WEAK_DEFAULT void isr_usbd(void);
WEAK_DEFAULT void isr_uarte1(void);
WEAK_DEFAULT void isr_qspi(void);
WEAK_DEFAULT void isr_cryptocell(void);
WEAK_DEFAULT void isr_spi3(void);
WEAK_DEFAULT void isr_pwm3(void);
#endif

#ifdef SOFTDEVICE_PRESENT
extern void SWI2_EGU2_IRQHandler(void);
#endif

/* interrupt vector table */
ISR_VECTORS const void *interrupt_vector[] = {
    /* Exception stack pointer */
    (void*) (&_estack),             /* pointer to the top of the stack */
    /* Cortex-M4 handlers */
    (void*) reset_handler_default,  /* entry point of the program */
    (void*) nmi_default,            /* non maskable interrupt handler */
    (void*) hard_fault_default,     /* hard fault exception */
    (void*) mem_manage_default,     /* memory manage exception */
    (void*) bus_fault_default,      /* bus fault exception */
    (void*) usage_fault_default,    /* usage fault exception */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_svc,                /* system call interrupt, in RIOT used for
                                     * switching into thread context on boot */
    (void*) debug_mon_default,      /* debug monitor exception */
    (void*) (0UL),                  /* Reserved */
    (void*) isr_pendsv,             /* pendSV interrupt, in RIOT the actual
                                     * context switching is happening here */
    (void*) isr_systick,            /* SysTick interrupt, not used in RIOT */
    /* nRF52 specific peripheral handlers */
    (void *) isr_power_clock,       /* power_clock */
    (void *) isr_radio,             /* radio */
    (void *) isr_uart0,             /* uart0 */
    (void *) isr_spi0_twi0,         /* spi0_twi0 */
    (void *) isr_spi1_twi1,         /* spi1_twi1 */
    (void *) isr_nfct,              /* nfct */
    (void *) isr_gpiote,            /* gpiote */
    (void *) isr_saadc,             /* adc */
    (void *) isr_timer0,            /* timer0 */
    (void *) isr_timer1,            /* timer1 */
    (void *) isr_timer2,            /* timer2 */
    (void *) isr_rtc0,              /* rtc0 */
    (void *) isr_temp,              /* temp */
    (void *) isr_rng,               /* rng */
    (void *) isr_ecb,               /* ecb */
    (void *) isr_ccm_aar,           /* ccm_aar */
    (void *) isr_wdt,               /* wdt */
    (void *) isr_rtc1,              /* rtc1 */
    (void *) isr_qdec,              /* qdec */
    (void *) isr_lpcomp,            /* lpcomp */
    (void *) isr_swi0,              /* swi0 */
    (void *) isr_swi1,              /* swi1 */
#ifdef SOFTDEVICE_PRESENT
    (void *) SWI2_EGU2_IRQHandler,  /* softdevice swi handler */
#else
    (void *) isr_swi2,              /* swi2 */
#endif
    (void *) isr_swi3,              /* swi3 */
    (void *) isr_swi4,              /* swi4 */
    (void *) isr_swi5,              /* swi5 */
    (void *) isr_timer3,            /* timer 3 */
    (void *) isr_timer4,            /* timer 4 */
    (void *) isr_pwm0,              /* pwm 0 */
    (void *) isr_pdm,               /* pdm */
    (void *)(0UL),                  /* reserved */
    (void *)(0UL),                  /* reserved */
    (void *) isr_mwu,               /* mwu */
    (void *) isr_pwm1,              /* pwm 1 */
    (void *) isr_pwm2,              /* pwm 2 */
    (void *) isr_spi2,              /* spi 2 */
    (void *) isr_rtc2,              /* rtc 2 */
    (void *) isr_i2s,               /* i2s */
#ifdef CPU_MODEL_NRF52840XXAA
    (void *) isr_fpu,               /* fpu */
    (void *) isr_usbd,              /* usbc */
    (void *) isr_uarte1,            /* uarte1 */
    (void *) isr_qspi,              /* qspi */
    (void *) isr_cryptocell,        /* cryptocell */
    (void *) isr_spi3,              /* spi3 */
    (void *) (0UL),                 /* reserved */
    (void *) isr_pwm3,              /* pwm3 */
#endif
};
