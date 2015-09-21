/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
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

/** Type for timer state */
typedef struct {
    void (*cb)(int);
} timer_conf_t;

/** Type for virtual count up timer */
typedef struct {
    uint32_t counter32b;
    uint32_t diff;
} count_up_timer_t;

static count_up_timer_t cu_timer[TIMER_NUMOF];

/** Timer state memory */
static timer_conf_t config[TIMER_NUMOF];

inline static void pit_timer_start(uint8_t ch)
{
    TIMER_BASE->CHANNEL[ch].TCTRL |= (PIT_TCTRL_TEN_MASK);
}

inline static void pit_timer_stop(uint8_t ch)
{
    TIMER_BASE->CHANNEL[ch].TCTRL &= ~(PIT_TCTRL_TEN_MASK);
}

/** use channel n-1 as prescaler */
inline static void timer_set_prescaler(uint8_t ch, unsigned int ticks_per_us)
{
    TIMER_BASE->CHANNEL[ch].TCTRL = 0x0;
    TIMER_BASE->CHANNEL[ch].LDVAL = (TIMER_CLOCK / 1e6) / ticks_per_us;
    TIMER_BASE->CHANNEL[ch].TCTRL = (PIT_TCTRL_TEN_MASK);
}

inline static void timer_set_counter(uint8_t ch)
{
    TIMER_BASE->CHANNEL[ch].TCTRL = 0x0;
    TIMER_BASE->CHANNEL[ch].LDVAL = TIMER_MAX_VALUE;
    TIMER_BASE->CHANNEL[ch].TCTRL = (PIT_TCTRL_TIE_MASK | PIT_TCTRL_CHN_MASK);
}

inline static uint32_t pit_timer_read(tim_t dev, uint8_t ch)
{
    return cu_timer[dev].counter32b + (TIMER_BASE->CHANNEL[ch].LDVAL
                                       - TIMER_BASE->CHANNEL[ch].CVAL);
}

inline static void pit_timer_set_max(uint8_t ch)
{
    pit_timer_stop(ch);
    TIMER_BASE->CHANNEL[ch].LDVAL = TIMER_MAX_VALUE;
    pit_timer_start(ch);
}

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int))
{
    PIT_Type *timer = TIMER_BASE;

    /* enable timer peripheral clock */
    TIMER_CLKEN();

    timer->MCR = PIT_MCR_FRZ_MASK;

    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            NVIC_SetPriority(TIMER_0_IRQ_CHAN, TIMER_IRQ_PRIO);
            timer_set_prescaler(TIMER_0_PRESCALER_CH, ticks_per_us);
            timer_set_counter(TIMER_0_COUNTER_CH);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            NVIC_SetPriority(TIMER_1_IRQ_CHAN, TIMER_IRQ_PRIO);
            timer_set_prescaler(TIMER_1_PRESCALER_CH, ticks_per_us);
            timer_set_counter(TIMER_1_COUNTER_CH);
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    /* set callback function */
    config[dev].cb = callback;
    cu_timer[dev].counter32b = 0;
    cu_timer[dev].diff = 0;

    /* enable the timer's interrupt */
    timer_irq_enable(dev);

    /* start the timer */
    timer_start(dev);

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    unsigned int now = timer_read(dev);

    return timer_set_absolute(dev, channel, now + timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    (void) channel; /* we only support one channel */
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            cu_timer[dev].counter32b = pit_timer_read(dev, TIMER_0_COUNTER_CH);
            pit_timer_stop(TIMER_0_COUNTER_CH);
            cu_timer[dev].diff = value - cu_timer[dev].counter32b;
            TIMER_BASE->CHANNEL[TIMER_0_COUNTER_CH].LDVAL = cu_timer[dev].diff;
            pit_timer_start(TIMER_0_COUNTER_CH);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            cu_timer[dev].counter32b = pit_timer_read(dev, TIMER_1_COUNTER_CH);
            pit_timer_stop(TIMER_1_COUNTER_CH);
            cu_timer[dev].diff = value - cu_timer[dev].counter32b;
            TIMER_BASE->CHANNEL[TIMER_1_COUNTER_CH].LDVAL = cu_timer[dev].diff;
            pit_timer_start(TIMER_1_COUNTER_CH);
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    DEBUG("cntr: %lu, value: %u, diff: %lu\n",
          cu_timer[dev].counter32b, value, cu_timer[dev].diff);

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    (void) channel; /* we only support one channel */
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            cu_timer[dev].counter32b = timer_read(dev);
            cu_timer[dev].diff = 0;
            pit_timer_set_max(TIMER_0_COUNTER_CH);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            cu_timer[dev].counter32b = timer_read(dev);
            cu_timer[dev].diff = 0;
            pit_timer_set_max(TIMER_1_COUNTER_CH);
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return -1;
    }

    DEBUG("clear\n");

    return 0;
}

unsigned int timer_read(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            return pit_timer_read(dev, TIMER_0_COUNTER_CH);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            return pit_timer_read(dev, TIMER_1_COUNTER_CH);
            break;
#endif

        case TIMER_UNDEFINED:
        default:
            return 0;
    }
}

void timer_start(tim_t dev)
{
    switch (dev) {
#if TIMER_0_EN

        case TIMER_0:
            pit_timer_start(TIMER_0_COUNTER_CH);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            pit_timer_start(TIMER_1_COUNTER_CH);
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
            pit_timer_stop(TIMER_0_COUNTER_CH);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            pit_timer_stop(TIMER_1_COUNTER_CH);
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
            NVIC_EnableIRQ(TIMER_0_IRQ_CHAN);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            NVIC_EnableIRQ(TIMER_1_IRQ_CHAN);
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
            NVIC_DisableIRQ(TIMER_0_IRQ_CHAN);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            NVIC_DisableIRQ(TIMER_1_IRQ_CHAN);
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
            pit_timer_set_max(TIMER_0_COUNTER_CH);
            break;
#endif
#if TIMER_1_EN

        case TIMER_1:
            pit_timer_set_max(TIMER_1_COUNTER_CH);
            break;
#endif

        case TIMER_UNDEFINED:
            break;
    }
}

inline static void pit_timer_irq_handler(tim_t dev, uint8_t ch)
{
    cu_timer[dev].counter32b += TIMER_BASE->CHANNEL[ch].LDVAL;

    TIMER_BASE->CHANNEL[ch].TFLG = PIT_TFLG_TIF_MASK;

    if (cu_timer[dev].diff) {
        if (config[dev].cb != NULL) {
            config[dev].cb(0);
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#if TIMER_0_EN
void TIMER_0_ISR(void)
{
    pit_timer_irq_handler(TIMER_0, TIMER_0_COUNTER_CH);
}
#endif

#if TIMER_1_EN
void TIMER_1_ISR(void)
{
    pit_timer_irq_handler(TIMER_1, TIMER_1_COUNTER_CH);
}
#endif

#endif
