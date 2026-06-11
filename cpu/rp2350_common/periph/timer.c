/*
 * SPDX-FileCopyrightText: 2026 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Timer implementation for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include <errno.h>

#include "compat_layer.h"
#include "irq.h"
#include "macros/units.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "timex.h"

#include "periph/timer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV(d)          (timer_config[d].dev)
#define ALARM(d, a)     ((&(DEV(d)->ALARM0)) + (a))

static timer_cb_t ctx_cb[TIMER_NUMOF];
static void *ctx_arg[TIMER_NUMOF];
static uint8_t flag_periodic[TIMER_NUMOF];
static uint8_t flag_reset[TIMER_NUMOF];

static void _timer_reset(tim_t dev)
{
    unsigned state = irq_disable();
    /* Always write TIMELW before TIMEHW, the value is only copied to the
     * counter once TIMEHW is written (see table 1229) */
    DEV(dev)->TIMELW = 0;
    DEV(dev)->TIMEHW = 0;
    irq_restore(state);
}

static void _isr(tim_t dev, int channel)
{
    /* The latched interrupt flags are write-1-clear (see table 1243) */
    DEV(dev)->INTR = 1u << channel;

    if (flag_periodic[dev] & (1u << channel)) {
        if (flag_reset[dev] & (1u << channel)) {
            _timer_reset(dev);
        }
        /* Bomb has been planted */
        *ALARM(dev, channel) = *ALARM(dev, channel);
    }

    if (ctx_cb[dev]) {
        ctx_cb[dev](ctx_arg[dev], channel);
    }
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    /* TICKS blocks are designed to generate 1 us ticks, so we can only support that frequency */
    assert(freq == US_PER_SEC);
    (void)freq;

    ctx_cb[dev] = cb;
    ctx_arg[dev] = arg;
    flag_periodic[dev] = 0;
    flag_reset[dev] = 0;

    /* Reset the timer block, so we can be sure it is in a known state */
    uint32_t reset_bit = (DEV(dev) == TIMER0) ? RESET_TIMER0 : RESET_TIMER1;
    reset_component(reset_bit, reset_bit);

    /* Generate one tick per microsecond from clk_ref, which runs from xosc */
    if (DEV(dev) == TIMER0) {
        TICKS->TIMER0_CYCLES = CLOCK_XOSC / MHZ(1);
        TICKS->TIMER0_CTRL = TICKS_CTRL_ENABLE_BITS;
    }
    else {
        TICKS->TIMER1_CYCLES = CLOCK_XOSC / MHZ(1);
        TICKS->TIMER1_CTRL = TICKS_CTRL_ENABLE_BITS;
    }

    for (uint8_t i = 0; i < timer_config[dev].ch_numof; i++) {
        rp_irq_enable(timer_config[dev].irqn_base + i);
        atomic_set(&DEV(dev)->INTE, 1u << i);
    }

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    if ((channel < 0) || (channel >= timer_config[dev].ch_numof)) {
        return -EINVAL;
    }

    if (!timeout) {
        /* catch case that a tick happens right before arming the alarm */
        if (ctx_cb[dev]) {
            ctx_cb[dev](ctx_arg[dev], channel);
        }
    }
    else {
        unsigned state = irq_disable();
        flag_periodic[dev] &= ~(1u << channel);
        /* An alarm matches on the lower 32 bits */
        *ALARM(dev, channel) = DEV(dev)->TIMERAWL + timeout;
        irq_restore(state);
    }

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    if ((channel < 0) || (channel >= timer_config[dev].ch_numof)) {
        return -EINVAL;
    }

    unsigned state = irq_disable();
    flag_periodic[dev] &= ~(1u << channel);
    *ALARM(dev, channel) = (uint32_t)value;
    irq_restore(state);

    return 0;
}

int timer_set_periodic(tim_t dev, int channel, unsigned int value, uint8_t flags)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    if ((channel < 0) || (channel >= timer_config[dev].ch_numof)) {
        return -EINVAL;
    }

    if (flags & TIM_FLAG_SET_STOPPED) {
        timer_stop(dev);
    }
    if (flags & TIM_FLAG_RESET_ON_SET) {
        _timer_reset(dev);
    }

    unsigned state = irq_disable();
    flag_periodic[dev] |= (1u << channel);
    if (flags & TIM_FLAG_RESET_ON_MATCH) {
        flag_reset[dev] |= (1u << channel);
    }
    else {
        flag_reset[dev] &= ~(1u << channel);
    }
    *ALARM(dev, channel) = (uint32_t)value;
    irq_restore(state);

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    if ((channel < 0) || (channel >= timer_config[dev].ch_numof)) {
        return -EINVAL;
    }

    unsigned state = irq_disable();
    /* The ARMED bits are write-1-clear (table 1236) */
    DEV(dev)->ARMED = 1u << channel;
    flag_periodic[dev] &= ~(1u << channel);
    irq_restore(state);

    return 0;
}

unsigned int timer_read(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    /* TIMERAWL reads the lower 32 bits without latching */
    return DEV(dev)->TIMERAWL;
}

void timer_start(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    atomic_clear(&DEV(dev)->PAUSE, 1u);
}

void timer_stop(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    atomic_set(&DEV(dev)->PAUSE, 1u);
}

void isr_timer0_0(void)
{
    _isr(0, 0);
    rp_end_isr();
}

void isr_timer0_1(void)
{
    _isr(0, 1);
    rp_end_isr();
}

void isr_timer0_2(void)
{
    _isr(0, 2);
    rp_end_isr();
}

void isr_timer0_3(void)
{
    _isr(0, 3);
    rp_end_isr();
}

void isr_timer1_0(void)
{
    _isr(1, 0);
    rp_end_isr();
}

void isr_timer1_1(void)
{
    _isr(1, 1);
    rp_end_isr();
}

void isr_timer1_2(void)
{
    _isr(1, 2);
    rp_end_isr();
}

void isr_timer1_3(void)
{
    _isr(1, 3);
    rp_end_isr();
}

/** @} */
