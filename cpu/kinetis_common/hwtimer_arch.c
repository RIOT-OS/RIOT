/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_hwtimer
 * @{
 *
 * @file
 * @brief       Implementation of the kernels hwtimer interface.
 *              hwtimer uses Freescale Low Power Timer lptmr0.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "hwtimer_cpu.h"
#include "cpu-conf.h"
#include "thread.h"

#include "debug.h"

#define LPTMR_MAXTICKS            (0x0000FFFF)

#ifndef LPTIMER_CNR_NEEDS_LATCHING
#warning LPTIMER_CNR_NEEDS_LATCHING is not defined in cpu-conf.h! Defaulting to 1
#define LPTIMER_CNR_NEEDS_LATCHING 1
#endif

typedef struct {
    uint32_t counter32b;
    uint32_t cmr32b;
    uint32_t diff;
} hwtimer_stimer32b_t;

static hwtimer_stimer32b_t stimer;

/**
 * @brief Reference to the hwtimer callback
 */
void (*timeout_handler)(int);

inline static uint32_t lptmr_get_cnr(void)
{
#if LPTIMER_CNR_NEEDS_LATCHING
    /* Write some garbage to CNR to latch the current value */
    LPTIMER_DEV->CNR = 42;
    return (uint32_t)LPTIMER_DEV->CNR;
#else
    /* The early revisions of the Kinetis CPUs do not need latching of the CNR
     * register. However, this may lead to corrupt values, we read it twice to
     * ensure that we got a valid value */
    int i;
    uint32_t tmp;
    uint32_t cnr = LPTIMER_DEV->CNR;

    /* you get three retries */
    for (i = 0; i < 3; ++i) {
        tmp = LPTIMER_DEV->CNR;

        if (tmp == cnr) {
            return cnr;
        }

        cnr = tmp;
    }

#endif
}

inline static void hwtimer_start(void)
{
    LPTIMER_DEV->CSR |= LPTMR_CSR_TEN_MASK;
}

inline static void hwtimer_stop(void)
{
    LPTIMER_DEV->CSR &= ~LPTMR_CSR_TEN_MASK;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    timeout_handler = handler;

    /* unlock LPTIMER_DEV */
    LPTIMER_CLKEN();
    /* set lptmr's IRQ priority */
    NVIC_SetPriority(LPTIMER_IRQ_CHAN, LPTIMER_IRQ_PRIO);
    /* reset lptmr */
    LPTIMER_DEV->CSR = 0;

    switch (LPTIMER_CLKSRC) {
        case LPTIMER_CLKSRC_MCGIRCLK:
            /* Select MCGIRCLK as clock source */
            LPTIMER_DEV->PSR = LPTMR_PSR_PRESCALE(LPTIMER_CLK_PRESCALE) | LPTMR_PSR_PCS(0);
            break;

        case LPTIMER_CLKSRC_OSCERCLK:
            /* Select OSCERCLK(4 MHz) as clock source */
            LPTIMER_DEV->PSR = LPTMR_PSR_PRESCALE(LPTIMER_CLK_PRESCALE) | LPTMR_PSR_PCS(3);
            break;

        case LPTIMER_CLKSRC_ERCLK32K:
            /* Select rtc oscillator output as clock source for ERCLK32K, */
            /* it needs functioning RTC module and driver. */
            SIM->SOPT1 &= ~(SIM_SOPT1_OSC32KSEL_MASK);
            SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(2);
            /* select ERCLK32K as clock source for lptmr0 */
            LPTIMER_DEV->PSR = LPTMR_PSR_PBYP_MASK | LPTMR_PSR_PCS(2);
            break;

        case LPTIMER_CLKSRC_LPO:
        default:
            /* select LPO as clock source (1 kHz)*/
            LPTIMER_DEV->PSR = LPTMR_PSR_PBYP_MASK | LPTMR_PSR_PCS(1);
    }

    LPTIMER_DEV->CMR = (uint16_t)(LPTMR_MAXTICKS);
    /* enable lptrm interrupt */
    LPTIMER_DEV->CSR = LPTMR_CSR_TIE_MASK;

    stimer.counter32b = 0;
    stimer.cmr32b = 0;
    stimer.diff = 0;

    hwtimer_arch_enable_interrupt();
    hwtimer_start();
}

void hwtimer_arch_enable_interrupt(void)
{
    NVIC_EnableIRQ(LPTIMER_IRQ_CHAN);
}

void hwtimer_arch_disable_interrupt(void)
{
    NVIC_DisableIRQ(LPTIMER_IRQ_CHAN);
}

void hwtimer_arch_set(unsigned long offset, short timer)
{
    (void)timer;
    stimer.counter32b += lptmr_get_cnr();
    hwtimer_stop();

    stimer.cmr32b = stimer.counter32b + offset;
    stimer.diff = offset;

    if (stimer.diff > LPTMR_MAXTICKS) {
        stimer.diff = LPTMR_MAXTICKS;
    }

    DEBUG("cntr: %lu, cmr: %lu, diff: %lu\n", stimer.counter32b, stimer.cmr32b, stimer.diff);

    LPTIMER_DEV->CMR = (uint16_t)(stimer.diff);
    hwtimer_start();
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    (void)timer;
    stimer.counter32b += lptmr_get_cnr();
    hwtimer_stop();

    stimer.cmr32b = value;
    stimer.diff = stimer.cmr32b - stimer.counter32b;

    if (stimer.diff > LPTMR_MAXTICKS) {
        stimer.diff = LPTMR_MAXTICKS;
    }

    DEBUG("cntr: %lu, cmr: %lu, diff: %lu\n", stimer.counter32b, stimer.cmr32b, stimer.diff);

    LPTIMER_DEV->CMR = (uint16_t)(stimer.diff);
    hwtimer_start();
}

void hwtimer_arch_unset(short timer)
{
    stimer.counter32b += lptmr_get_cnr();
    hwtimer_stop();
    stimer.diff = 0;
    stimer.cmr32b = 0;
    LPTIMER_DEV->CMR = (uint16_t)(LPTMR_MAXTICKS);
    hwtimer_start();

}

unsigned long hwtimer_arch_now(void)
{
    return (unsigned int)((lptmr_get_cnr() + stimer.counter32b));
}

void isr_lptmr0(void)
{
    stimer.counter32b += (uint32_t)LPTIMER_DEV->CMR;
    /* clear compare flag (w1c bit) */
    LPTIMER_DEV->CSR |= LPTMR_CSR_TCF_MASK;

    if (stimer.diff) {
        if (stimer.cmr32b > stimer.counter32b) {
            hwtimer_arch_set_absolute(stimer.cmr32b, 0);
        }
        else {
            stimer.diff = 0;
            timeout_handler((short)0);
        }
    }
    else {
        hwtimer_arch_unset(0);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
