/*
 * Copyright (C) 2016 Eistec AB
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014-2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_timer
 *
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdlib.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if TIMER_NUMOF

#ifndef TIMER_LPTMR_WITH_RTC
/** @brief Combine LPTMR and RTC by using RTC module prescaler counter for
 * timer_read() and use LPTMR for setting timers. (Default: 1)
 *
 * The rationale is that by letting the RTC run the readings we should never
 * lose any ticks. */
#define TIMER_LPTMR_WITH_RTC    1
#endif

/*
 * The RTC prescaler will normally count to 32767 every second unless configured
 * otherwise through the time compensation register.
 */
#define TIMER_RTC_SUBTICK_MAX (0x7fff)
/*
 * Number of bits in the ideal RTC prescaler counter
 */
#define TIMER_RTC_SUBTICK_BITS (15)

#ifndef LPTIMER_CNR_NEEDS_LATCHING
#warning LPTIMER_CNR_NEEDS_LATCHING is not defined in cpu_conf.h! Defaulting to 1
#define LPTIMER_CNR_NEEDS_LATCHING 1
#endif

#define LPTMR_BASE_FREQ (32768ul)

/** Type for timer state */
typedef struct {
    void (*cb)(int);
} timer_conf_t;

/** Timer state memory */
static timer_conf_t config[TIMER_NUMOF];

/** Reference count from RTC for time LPTMR CNR = 0 */
static uint32_t lptmr_reference = 0;

/* ********************** */
/* PIT handling functions */
/* ********************** */

inline static void pit_start(uint8_t ch)
{
    BITBAND_REG32(TIMER_PIT_DEV->CHANNEL[ch].TCTRL, PIT_TCTRL_TEN_SHIFT) = 1;
}

inline static void pit_stop(uint8_t ch)
{
    BITBAND_REG32(TIMER_PIT_DEV->CHANNEL[ch].TCTRL, PIT_TCTRL_TEN_SHIFT) = 0;
}

/** use channel n-1 as prescaler */
inline static void pit_set_prescaler(uint8_t ch, unsigned long freq)
{
    TIMER_PIT_DEV->CHANNEL[ch].TCTRL = 0x0;
    TIMER_PIT_DEV->CHANNEL[ch].LDVAL = (TIMER_PIT_FREQ / freq) - 1;
    TIMER_PIT_DEV->CHANNEL[ch].TCTRL = (PIT_TCTRL_TEN_MASK);
}

inline static void pit_set_counter(uint8_t ch, uint32_t timeout)
{
    TIMER_PIT_DEV->CHANNEL[ch].TCTRL = 0x0;
    TIMER_PIT_DEV->CHANNEL[ch].LDVAL = timeout;
    TIMER_PIT_DEV->CHANNEL[ch].TCTRL = (PIT_TCTRL_TIE_MASK | PIT_TCTRL_CHN_MASK);
}

inline static uint32_t pit_read(uint8_t ch)
{
    return (TIMER_PIT_DEV->CHANNEL[ch].LDVAL - TIMER_PIT_DEV->CHANNEL[ch].CVAL);
}

#if TIMER_1_EN
static int pit_init(tim_t dev, unsigned long freq, void (*callback)(int)) {
    /* enable timer peripheral clock */
    TIMER_PIT_CLKEN();

    TIMER_PIT_DEV->MCR = PIT_MCR_FRZ_MASK;
    switch (dev) {
#if TIMER_1_EN
        case TIMER_1:
            NVIC_SetPriority(TIMER_1_IRQ_CHAN, TIMER_IRQ_PRIO);
            pit_set_prescaler(TIMER_1_PRESCALER_CH, freq);
            break;
#endif
#if TIMER_2_EN
        case TIMER_2:
            NVIC_SetPriority(TIMER_2_IRQ_CHAN, TIMER_IRQ_PRIO);
            pit_set_prescaler(TIMER_2_PRESCALER_CH, freq);
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* set callback function */
    config[dev].cb = callback;

    /* enable the timer's interrupt */
    timer_irq_enable(dev);

    return 0;
}
#endif

/* ************************ */
/* LPTMR handling functions */
/* ************************ */

inline static void lptmr_start(void) {
    BITBAND_REG32(TIMER_LPTMR_DEV->CSR, LPTMR_CSR_TEN_SHIFT) = 1;
}

inline static void lptmr_stop(void) {
    BITBAND_REG32(TIMER_LPTMR_DEV->CSR, LPTMR_CSR_TEN_SHIFT) = 0;
}

inline static void lptmr_irq_enable(void) {
    NVIC_EnableIRQ(TIMER_LPTMR_IRQ_CHAN);
}

inline static void lptmr_irq_disable(void) {
    NVIC_DisableIRQ(TIMER_LPTMR_IRQ_CHAN);
}

#if TIMER_LPTMR_WITH_RTC
/** @brief Read the prescaler register from the RTC as a reliable 47 bit time
 * counter.
 */
inline static uint32_t rtt_get_subtick(void)
{
    uint32_t tpr;
    uint32_t tsr;

    for (int i = 0; i < 5; i++) {
        /* Read twice to make sure we get a stable reading */
        tpr = RTC->TPR & RTC_TPR_TPR_MASK;
        tsr = RTC->TSR & RTC_TSR_TSR_MASK;

        if ((tsr == (RTC->TSR & RTC_TSR_TSR_MASK)) &&
            (tpr == (RTC->TPR & RTC_TPR_TPR_MASK))) {
            break;
        }
    }
    if (tpr > TIMER_RTC_SUBTICK_MAX) {
        /* This only happens if the RTC time compensation value has been
         * modified to compensate for RTC drift. See Kinetis ref.manual,
         *  RTC Time Compensation Register (RTC_TCR).
         */
        tpr = TIMER_RTC_SUBTICK_MAX;
    }

    return (tsr << TIMER_RTC_SUBTICK_BITS) | tpr;
}
#endif

inline static void lptmr_set(uint16_t timeout) {
    lptmr_stop();
    TIMER_LPTMR_DEV->CMR = LPTMR_CMR_COMPARE(timeout);
    /* Enable LPTMR compare interrupt source */
    BITBAND_REG32(TIMER_LPTMR_DEV->CSR, LPTMR_CSR_TIE_SHIFT) = 1;
#if TIMER_LPTMR_WITH_RTC
    lptmr_reference = rtt_get_subtick();
#endif
    lptmr_start();
}

inline static unsigned int lptmr_read(void) {
    uint32_t ticks;
#if LPTIMER_CNR_NEEDS_LATCHING
    /* Write some garbage to CNR to latch the value of the counter */
    TIMER_LPTMR_DEV->CNR = 0;
    ticks = TIMER_LPTMR_DEV->CNR & LPTMR_CNR_COUNTER_MASK;
#else
    for (int i = 0; i < 5; i++) {
        /* Read twice to make sure we get a stable reading */
        ticks = TIMER_LPTMR_DEV->CNR & LPTMR_CNR_COUNTER_MASK;

        if (ticks == (TIMER_LPTMR_DEV->CNR & LPTMR_CNR_COUNTER_MASK)) {
            break;
        }
    }
#endif

    return (unsigned int)((ticks + lptmr_reference) & 0xffff);
}

/* Compute the prescaler setting, see reference manual for details */
inline static int lptmr_compute_prescaler(unsigned long freq) {
    int prescale = 0;
    if ((freq > LPTMR_BASE_FREQ) || (freq == 0)) {
        return -1;
    }
    while (freq < LPTMR_BASE_FREQ){
        ++prescale;
        freq <<= 1;
    }
    if (freq != LPTMR_BASE_FREQ) {
        /* not a power of two */
        return -2;
    }
    return prescale;
}

inline static int lptmr_init(tim_t dev, unsigned long freq, void (*callback)(int)) {
    int prescale = lptmr_compute_prescaler(freq);
    if (prescale < 0) {
        return -1;
    }
    TIMER_LPTMR_CLKEN();
    NVIC_SetPriority(TIMER_LPTMR_IRQ_CHAN, TIMER_IRQ_PRIO);

    /* reset LPTMR module */
    TIMER_LPTMR_DEV->CSR = 0;

    /* Select RTC oscillator output as clock source for ERCLK32K. */
    /* This requires a functioning RTC module and driver. */
    SIM->SOPT1 &= ~(SIM_SOPT1_OSC32KSEL_MASK);
    SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(2);
    /* select ERCLK32K as clock source for LPTMR */
    TIMER_LPTMR_DEV->PSR = LPTMR_PSR_PBYP_MASK | LPTMR_PSR_PCS(2) |
            LPTMR_PSR_PRESCALE((uint32_t)prescale);

    config[dev].cb = callback;

    /* enable free running counter mode */
    LPTIMER_DEV->CSR = LPTMR_CSR_TFC_MASK;

    lptmr_irq_enable();
#if TIMER_LPTMR_WITH_RTC
    lptmr_reference = rtt_get_subtick();
#endif
    lptmr_start();
    return 0;
}

int timer_init(tim_t dev, unsigned long freq, void (*callback)(int))
{

    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            return lptmr_init(dev, freq, callback);
#endif
#if TIMER_1_EN

        case TIMER_1:
            return pit_init(dev, freq, callback);
#endif
#if TIMER_2_EN

        case TIMER_2:
            return pit_init(dev, freq, callback);
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    /* we only support one channel */
    if (channel != 0) {
        return -1;
    }

    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            lptmr_set(timeout);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            pit_set_counter(TIMER_1_COUNTER_CH, timeout);
            pit_start(TIMER_1_COUNTER_CH);
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            pit_set_counter(TIMER_2_COUNTER_CH, timeout);
            pit_start(TIMER_2_COUNTER_CH);
            break;
#endif
        default:
            return -1;
    }

    return 1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    /* we only support one channel */
    if (channel != 0) {
        return -1;
    }

    unsigned int now;

    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            now = lptmr_read();
            lptmr_set(value - now);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            now = pit_read(channel);
            pit_set_counter(TIMER_1_COUNTER_CH, value - now);
            pit_start(TIMER_1_COUNTER_CH);
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            now = pit_read(channel);
            pit_set_counter(TIMER_2_COUNTER_CH, value - now);
            pit_start(TIMER_2_COUNTER_CH);
            break;
#endif
        default:
            return -1;
    }

    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    /* we only support one channel */
    if (channel != 0) {
        return -1;
    }

    timer_stop(dev);

    return 0;
}

unsigned int timer_read(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            return lptmr_read();
#endif
#if TIMER_1_EN

        case TIMER_1:
            return pit_read(TIMER_1_COUNTER_CH);
#endif
#if TIMER_2_EN

        case TIMER_2:
            return pit_read(TIMER_2_COUNTER_CH);
#endif
        default:
            return 0;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            lptmr_start();
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            pit_start(TIMER_1_COUNTER_CH);
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            pit_start(TIMER_2_COUNTER_CH);
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_stop(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            lptmr_stop();
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            pit_stop(TIMER_1_COUNTER_CH);
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            pit_stop(TIMER_2_COUNTER_CH);
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_enable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            lptmr_irq_enable();
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            NVIC_EnableIRQ(TIMER_1_IRQ_CHAN);
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            NVIC_EnableIRQ(TIMER_2_IRQ_CHAN);
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_irq_disable(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            lptmr_irq_disable();
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQ_CHAN);
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            NVIC_DisableIRQ(TIMER_2_IRQ_CHAN);
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

void timer_reset(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            lptmr_stop();
            lptmr_start();
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            pit_stop(TIMER_1_COUNTER_CH);
            pit_start(TIMER_1_COUNTER_CH);
            break;
#endif
#if TIMER_2_EN

        case TIMER_2:
            pit_stop(TIMER_2_COUNTER_CH);
            pit_start(TIMER_2_COUNTER_CH);
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

inline static void pit_irq_handler(tim_t dev, uint8_t ch)
{
    /* Stop timer */
    pit_stop(ch);
    /* Clear interrupt flag */
    TIMER_PIT_DEV->CHANNEL[ch].TFLG = PIT_TFLG_TIF_MASK;

    if (config[dev].cb != NULL) {
        config[dev].cb(0);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef TIMER_0_EN
void TIMER_LPTMR_ISR(void) {
    static const tim_t dev = TIMER_0;

    /* Disable LPTMR interrupt until a new timeout has been set */
    BITBAND_REG32(TIMER_LPTMR_DEV->CSR, LPTMR_CSR_TIE_SHIFT) = 0;

    /* Clear interrupt flag */
    BITBAND_REG32(TIMER_LPTMR_DEV->CSR, LPTMR_CSR_TCF_SHIFT) = 1;

    if (config[dev].cb != NULL) {
        config[dev].cb(0);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if TIMER_1_EN
void TIMER_1_ISR(void)
{
    pit_irq_handler(TIMER_1, TIMER_1_COUNTER_CH);
}
#endif

#if TIMER_2_EN
void TIMER_2_ISR(void)
{
    pit_irq_handler(TIMER_2, TIMER_2_COUNTER_CH);
}
#endif

#endif
