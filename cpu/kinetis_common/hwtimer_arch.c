/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
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
 * @brief       Implementation of the kernel hwtimer interface.
 *              hwtimer uses Freescale Low Power Timer LPTMR0.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "arch/hwtimer_arch.h"
#include "hwtimer_cpu.h"
#include "cpu_conf.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define ENABLE_STATS (0)

#if ENABLE_STATS
/* Collect some statistics */
struct {
    unsigned int count_set_total;
    unsigned int count_set_absolute;
    unsigned int count_unset_total;
    unsigned int count_isr_total;
    unsigned int count_isr_rollover;
    unsigned int count_isr_handler_call;
    unsigned int count_isr_unset;
} hwtimer_stats = {
        .count_set_total = 0,
        .count_set_absolute = 0,
        .count_unset_total = 0,
        .count_isr_total = 0,
        .count_isr_rollover = 0,
        .count_isr_handler_call = 0,
        .count_isr_unset = 0,
};

#define HWTIMER_STATS_INC(x) (hwtimer_stats.x++)

#else
#define HWTIMER_STATS_INC(x)
#endif /* HWTIMER_STATS */

#define LPTMR_MAXTICKS            (0x0000FFFF)

/* Minimum number of ticks when overflowing. This number should be calibrated
 * against the maximum run time of the LPTMR ISR in order to avoid missing a
 * target. */
#define LPTMR_ISR_TICK_MARGIN     (1)

/* Convenience macro to get a reference to the TCF flag in the status register */
#define LPTIMER_TCF (BITBAND_REG32(LPTIMER_DEV->CSR, LPTMR_CSR_TCF_SHIFT))


#ifndef LPTIMER_CNR_NEEDS_LATCHING
#warning LPTIMER_CNR_NEEDS_LATCHING is not defined in cpu_conf.h! Defaulting to 1
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

#if LPTIMER_CNR_NEEDS_LATCHING

/* read the CNR register */
inline static uint32_t lptmr_get_cnr(void)
{
    uint32_t cnr;
    /* Write some garbage to CNR to latch the current value */
    asm volatile (
        /* write garbage to CNR to latch the value, it does not matter what the
         * value of r0 is. Testing shows that the write must be 32 bit wide or
         * we will get garbage when reading back CNR. */
        "str %[CNR], [%[CNR_p], #0]\n"
        "ldr %[CNR], [%[CNR_p], #0]\n" /* Read CNR */
        : /* Output variables */
        [CNR] "=&r" (cnr)
        : /* Input variables */
        [CNR_p] "r" (&(LPTIMER_DEV->CNR))
        : /* Clobbered registers */
        );
    return cnr;
}

/* Update the CMR register and return the old CNR before the reset */
inline static uint32_t lptmr_update_cmr(uint32_t cmr)
{
    /* Optimized version for CPUs which support latching the LPTMR value */
    uint32_t cnr;
    asm volatile (
        /* Prepare On/Off values in registers r6, r7 */
        "mov r6, #0\n"
        "mov r7, #1\n"
        /* Any ticks that occur between the below write to CNR_p and the write
         * to TEN below will be lost, try to keep it short to minimize the
         * risk. */
        /* write garbage to CNR to latch the value, it does not matter what the
         * value of r7 is. Testing shows that the write must be 32 bit wide or
         * we will get garbage when reading back CNR. */
        "str r7, [%[CNR_p], #0]\n"
        "ldr %[CNR], [%[CNR_p], #0]\n" /* Read CNR */
        "strh r6, [%[TEN], #0]\n"       /* disable timer */
        "str %[CMR], [%[CMR_p], #0]\n" /* update CMR */
        "strh r7, [%[TEN], #0]\n"       /* enable timer */
        : /* Output variables */
        [CNR] "=&r" (cnr)
        : /* Input variables */
        [TEN] "r" (BITBAND_REGADDR(LPTIMER_DEV->CSR, LPTMR_CSR_TEN_SHIFT)),
        [CMR_p] "r" (&(LPTIMER_DEV->CMR)), [CMR] "r" (cmr),
        [CNR_p] "r" (&(LPTIMER_DEV->CNR))
        : /* Clobbered registers */
        "r6", "r7");
    /* The reset happens synchronously with regards to the LPTMR clock, this
     * means that on the next tick the CNR register will be reset to zero, and
     * not until the next tick after that will the CNR register increment, i.e.
     * one tick will always be lost.
     * In order to keep the hwtimer somewhat in sync with the RTT the user should
     * add 1 to the 32 bit software tick counter after each reset of the LPTMR. */
    return cnr;
}

#else /* LPTIMER_CNR_NEEDS_LATCHING */

/* read the CNR register */
inline static uint32_t lptmr_get_cnr(void)
{
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
    return cnr;
}

inline static uint32_t lptmr_update_cmr(uint32_t cmr)
{
    /* Unoptimized version for older CPUs, this will yield a greater timer drift
     * over time between the hwtimer_now() and rtt_get_counter() than the
     * optimized version above. */
    uint32_t cnr = lptmr_get_cnr();
    BITBAND_REG32(LPTIMER_DEV->CSR, LPTMR_CSR_TEN_SHIFT) = 1;
    LPTIMER_DEV->CMR = cmr;
    BITBAND_REG32(LPTIMER_DEV->CSR, LPTMR_CSR_TEN_SHIFT) = 0;
    return cnr;
}
#endif /* LPTIMER_CNR_NEEDS_LATCHING */

inline static void hwtimer_start(void)
{
    BITBAND_REG32(LPTIMER_DEV->CSR, LPTMR_CSR_TEN_SHIFT) = 1;
}

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu)
{
    (void) fcpu; /* fcpu does not affect the Low power timer module frequency */
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
    /* enable LPTMR interrupt */
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
    (void)timer; /* we only support one timer */
    unsigned mask = disableIRQ();
    HWTIMER_STATS_INC(count_set_total);
    /* it is important that we don't get interrupted between stopping and
     * starting the timer again in order to not increase the risk of dropping
     * any ticks */
    stimer.diff = offset;

    if (stimer.diff > LPTMR_MAXTICKS) {
        stimer.diff = LPTMR_MAXTICKS;
    }

    uint32_t diff = stimer.diff;
    uint32_t cnr = lptmr_update_cmr(diff);
    stimer.counter32b += cnr;
    /* The reset in lptmr_update_cmr above happens synchronously and takes
     * one LPTMR clock cycle, add the lost tick to the counter to
     * compensate. */
    stimer.counter32b += 1;

    stimer.cmr32b = stimer.counter32b + offset;

    restoreIRQ(mask);

    DEBUG("cntr: %lu, cmr: %lu, diff: %lu\n", stimer.counter32b, stimer.cmr32b, stimer.diff);
}

void hwtimer_arch_set_absolute(unsigned long value, short timer)
{
    unsigned long offset = value - (stimer.counter32b + lptmr_get_cnr());
    HWTIMER_STATS_INC(count_set_absolute);
    return hwtimer_arch_set(offset, timer);
}

void hwtimer_arch_unset(short timer)
{
    (void)timer; /* we only support one timer */
    unsigned mask = disableIRQ();
    HWTIMER_STATS_INC(count_unset_total);
    stimer.diff = 0;
    restoreIRQ(mask);
}

unsigned long hwtimer_arch_now(void)
{
    return (unsigned int)((lptmr_get_cnr() + stimer.counter32b));
}

void isr_lptmr0(void)
{
    /* The timer counter is reset to 0 when the compare value is hit, add the
     * compare value to the 32 bit counter. Add 1 for counting the 0'th tick as
     * well (TCF asserts when CNR equals CMR and the counter increases) */
    stimer.counter32b += LPTIMER_DEV->CMR + 1;
    HWTIMER_STATS_INC(count_isr_total);

    /* We try to keep the timer running so that we don't lose any ticks. */
    /* Writing the CMR register is allowed when the timer is running
     * only if the TCF flag is asserted, therefore we defer clearing the TCF
     * flag until the end of this ISR */
    if (stimer.diff != 0) {
        if (stimer.cmr32b > stimer.counter32b) {
            /* Target time lies in the future, we rolled over and must update
             * the compare register */
            HWTIMER_STATS_INC(count_isr_rollover);
            stimer.diff = stimer.cmr32b - stimer.counter32b;

            if (stimer.diff < (lptmr_get_cnr() + LPTMR_ISR_TICK_MARGIN)) {
                /* We have already passed the target time after rolling over */
                /* This can happen if for example other ISRs delay the execution
                 * of this ISR */
                /* clear timer compare flag (w1c bit) */
                LPTIMER_TCF = 1;
                stimer.diff = 0;
                timeout_handler(0);
            } else {
                if (stimer.diff > LPTMR_MAXTICKS) {
                    /* Target time is too far away, we will have to roll over once
                     * more. */
                    stimer.diff = LPTMR_MAXTICKS;
                }
                LPTIMER_DEV->CMR = stimer.diff;
                /* clear compare flag (w1c bit) */
                LPTIMER_TCF = 1;
            }
        } else {
            HWTIMER_STATS_INC(count_isr_handler_call);
            /* clear compare flag (w1c bit) */
            LPTIMER_TCF = 1;
            /* Disable callback by setting diff=0 */
            stimer.diff = 0;
            timeout_handler(0);
        }
    } else {
        HWTIMER_STATS_INC(count_isr_unset);
        /* standby run to keep hwtimer_now increasing */
        LPTIMER_DEV->CMR = LPTMR_MAXTICKS;
        /* clear compare flag (w1c bit) */
        LPTIMER_TCF = 1;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
