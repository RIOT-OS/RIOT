/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <assert.h>
#include <stddef.h>

#include "cpu.h"
#include "periph/timer.h"
#include "periph_gba.h"

/**
 * @brief   Interrupt context information for configured timers
 */
static timer_isr_ctx_t isr_ctx[4];
static uint16_t timer_one_shot[4];
static uint16_t timer_start_value[4];
static uint16_t timer_last_value[4];

/**
 * @brief   Get the base pointer of a timer
 */
static inline gba_timer_t *get_dev(tim_t tim)
{
    switch (tim) {
    case 0:
        return GBA_TIMER_0;
    case 1:
        return GBA_TIMER_1;
    case 2:
        return GBA_TIMER_2;
    case 3:
        return GBA_TIMER_3;
    default:
        return NULL;
    }
}

void gba_timer_init(void)
{
    GBA_TM0_CNT_L = 0;
    GBA_TM0_CNT_H = 0;
    GBA_TM1_CNT_L = 0;
    GBA_TM1_CNT_H = 0;
    GBA_TM2_CNT_L = 0;
    GBA_TM2_CNT_H = 0;
    GBA_TM3_CNT_L = 0;
    GBA_TM3_CNT_H = 0;

    GBA_IE = GBA_IE_TIMERS; /* enable timer interrupt */
}

void gba_timer_isr(uint16_t interrupt_flags_timer)
{
    /* timers start at bit position 3 */
    interrupt_flags_timer >>= 3;

    int timer_num = 0;
    while (timer_num < 4 && interrupt_flags_timer > 0) {
        if (interrupt_flags_timer & 1) {
            gba_timer_t *dev = get_dev(timer_num);
            if (timer_one_shot[timer_num]) {
                /* turn off the timer to apply a new value later on */
                dev->run = 0;
                /* call the call back */
                isr_ctx[timer_num].cb(isr_ctx[timer_num].arg, 0);
                /* disable this one shot timer. Since we need the timer itself running */
                /* to continue to track the time, we need to prevent the next overflow */
                /* from doing stuff during the IRQ / ISR */
                timer_one_shot[timer_num] = 0;
                dev->irq_enable = 0;
                /* Add the passed time to our inter-irq-time-tracker */
                timer_last_value[timer_num] += GBA_TIMER_MAX_TICKS - timer_start_value[timer_num];
                /* Start the timer from zero again, makes the math easier + fewer IRQs */
                timer_start_value[timer_num] = 0;
                dev->current_value = 0;
                /* Enable the timer, this will apply the new timer value we just set */
                dev->run = 1;
            }
        }
        timer_num++;
        interrupt_flags_timer >>= 1;
    }
}

int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
    /* get the timers register access */
    gba_timer_t *dev = get_dev(tim);

    /* make sure the timer device is valid */
    if (dev == NULL) {
        return -1;
    }

    /* ensure the timer is off, valid state */
    dev->run = 0;

    /* If you want to use this mode, configure the timer yourself */
    dev->count_up_on_neighbour_overflow = 0;

    switch (freq) {
    case 16780000:
        dev->prescaler = 0;
        break;
    case 262200:
        dev->prescaler = 1;
        break;
    case 65500:
        dev->prescaler = 2;
        break;
    case 16400:
        dev->prescaler = 3;
        break;
    default:
        return -2;
    }

    /* save the callback */
    isr_ctx[tim].cb = cb;
    isr_ctx[tim].arg = arg;

    dev->current_value = 0;

    timer_last_value[tim] = 0;
    timer_start_value[tim] = 0;
    timer_one_shot[tim] = 1;

    dev->irq_enable = 1;

    /* start the timer */
    dev->run = 1;

    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    (void)channel;

    /* Interrupt will only be generated on overflow,
       so a 0 timeout is not possible */
    if (timeout < 5) {
        timeout = 5;
    }

    /* get the timers register access */
    gba_timer_t *dev = get_dev(tim);

    /* make sure the timer device is valid */
    if (dev == NULL) {
        return -1;
    }

    dev->run = 0;

    timer_last_value[tim] = timer_read(tim);

    /* we only get interrupts on overflow, reaching timeout must result in overflow */
    dev->current_value = GBA_TIMER_MAX_TICKS - (uint16_t)timeout;

    /* Remember the start value, we need that later when reading out the timer */
    timer_start_value[tim] = GBA_TIMER_MAX_TICKS - (uint16_t)timeout;

    dev->irq_enable = 1;
    timer_one_shot[tim] = 1;

    /* start the timer */
    dev->run = 1;

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    /* we don't have any channels or marker values, setting anything means */
    /* repurposing the entire timer. That is why we have to transform the  */
    /* absolute value into a relative timeout. */

    uint16_t relative_value_to_set = 0;
    uint16_t current_value = timer_read(dev);
    if (current_value < value) {
        relative_value_to_set = value - current_value;
    }
    else if (current_value > value) {
        relative_value_to_set = value + GBA_TIMER_MAX_TICKS - current_value;
    }
    else {
        relative_value_to_set = GBA_TIMER_MAX_TICKS;
    }
    return timer_set(dev, channel, relative_value_to_set);
}

int timer_clear(tim_t tim, int channel)
{
    assert(channel == 0);

    /* get the timers register access */
    gba_timer_t *dev = get_dev(tim);

    /* make sure the timer device is valid */
    if (dev == NULL) {
        return -1;
    }

    timer_one_shot[tim] = 0;
    dev->irq_enable = 0;

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    /* get the timers register access */
    gba_timer_t *dev = get_dev(tim);

    /* make sure the timer device is valid */
    if (dev == NULL) {
        return -1;
    }

    /* Get mmapped value into cpu register, so it doesn't change during calculation */
    uint16_t current_value = dev->current_value;
    uint16_t ret = timer_last_value[tim];

    if (timer_one_shot[tim]) {
        if (current_value >= timer_start_value[tim]) {
            /* The timer api implicitly expects to counting up towards the timeout. */
            /* Since we have to rewrite the current value to implement the timeout, */
            /* any naive read out, to determine how far we are from the timeout,    */
            /* would return an unexpected high value. Therefore we correct for it.  */
            ret += current_value - timer_start_value[tim];
        }
    }
    else {
        ret += current_value;
    }

    return ret;
}

void timer_start(tim_t tim)
{
    /* get the timers register access */
    gba_timer_t *dev = get_dev(tim);

    /* make sure the timer device is valid */
    if (dev != NULL) {
        dev->run = 1;
    }
}

void timer_stop(tim_t tim)
{
    /* get the timers register access */
    gba_timer_t *dev = get_dev(tim);

    /* make sure the timer device is valid */
    if (dev != NULL) {
        dev->run = 0;
    }
}

uword_t timer_query_freqs_numof(tim_t dev)
{
    (void)dev;
    return 4;
}

uword_t timer_query_channel_numof(tim_t dev)
{
    (void)dev;
    return 1;
}

uint32_t timer_query_freqs(tim_t dev, uword_t index)
{
    (void)dev;

    switch (index) {
    case 3:
        return 16400;
    case 2:
        return 65500;
    case 1:
        return 262200;
    case 0:
        return 16780000;
    default:
        return 0;
    }
}
