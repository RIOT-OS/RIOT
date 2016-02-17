/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral timer interface
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "sched.h"
#include "thread.h"
#include "periph/timer.h"

#define F_TIMER             (16000000U)     /* the timer is clocked at 16MHz */

typedef struct {
    timer_cb_t cb;
    void *arg;
    uint8_t flags;
} tim_ctx_t;

/**
 * @brief timer state memory
 */
static tim_ctx_t ctx[TIMER_NUMOF];

static inline NRF_TIMER_Type *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* make sure the given timer is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* save interrupt context */
    ctx[tim].cb = cb;
    ctx[tim].arg = arg;

    /* power on timer */
#if CPU_FAM_NRF51
    dev(tim)->POWER = 1;
#endif

    /* reset and configure the timer */
    dev(tim)->TASKS_STOP = 1;
    dev(tim)->BITMODE = timer_config[tim].bitmode;
    dev(tim)->MODE = TIMER_MODE_MODE_Timer;
    dev(tim)->TASKS_CLEAR = 1;

    /* figure out if desired frequency is available */
    int i;
    unsigned long cando = F_TIMER;
    for (i = 0; i < 10; i++) {
        if (freq == cando) {
            dev(tim)->PRESCALER = i;
            break;
        }
        cando /= 2;
    }
    if (i == 10) {
        return -1;
    }

    /* reset compare state */
    dev(tim)->EVENTS_COMPARE[0] = 0;
    dev(tim)->EVENTS_COMPARE[1] = 0;
    dev(tim)->EVENTS_COMPARE[2] = 0;

    /* enable interrupts */
    timer_irq_enable(tim);
    /* start the timer */
    dev(tim)->TASKS_START = 1;

    return 0;
}

int timer_set(tim_t tim, int chan, unsigned int value)
{
    uint32_t now = timer_read(tim);
    return timer_set_absolute(tim, chan, (now + value));
}

int timer_set_absolute(tim_t tim, int chan, unsigned int value)
{
    /* see if channel is valid */
    if (chan >= timer_config[tim].channels) {
        return -1;
    }

    ctx[tim].flags |= (1 << chan);
    dev(tim)->CC[chan] = value;
    dev(tim)->INTENSET = (TIMER_INTENSET_COMPARE0_Msk << chan);

    return 1;
}

int timer_clear(tim_t tim, int chan)
{
    /* see if channel is valid */
    if (chan >= timer_config[tim].channels) {
        return -1;
    }

    dev(tim)->INTENCLR = (TIMER_INTENSET_COMPARE0_Msk << chan);
    ctx[tim].flags &= ~(1 << chan);

    return 1;
}

unsigned int timer_read(tim_t tim)
{
    dev(tim)->TASKS_CAPTURE[timer_config[tim].channels] = 1;
    return dev(tim)->CC[timer_config[tim].channels];
}

void timer_start(tim_t tim)
{
    dev(tim)->TASKS_START = 1;
}

void timer_stop(tim_t tim)
{
    dev(tim)->TASKS_STOP = 1;
}

void timer_irq_enable(tim_t tim)
{
    NVIC_EnableIRQ(timer_config[tim].irqn);
}

void timer_irq_disable(tim_t tim)
{
    NVIC_DisableIRQ(timer_config[tim].irqn);
}

static inline void irq_handler(int num)
{
    for (unsigned i = 0; i < timer_config[num].channels; i++) {
        if (dev(num)->EVENTS_COMPARE[i] == 1) {
            dev(num)->EVENTS_COMPARE[i] = 0;
            if (ctx[num].flags & (1 << i)) {
                ctx[num].flags &= ~(1 << i);
                dev(num)->INTENCLR = (TIMER_INTENSET_COMPARE0_Msk << i);
                ctx[num].cb(ctx[num].arg, i);
            }
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef TIMER_0_ISR
void TIMER_0_ISR(void)
{
    irq_handler(0);
}
#endif

#ifdef TIMER_1_ISR
void TIMER_1_ISR(void)
{
    irq_handler(1);
}
#endif

#ifdef TIMER_2_ISR
void TIMER_2_ISR(void)
{
    irq_handler(2);
}
#endif
