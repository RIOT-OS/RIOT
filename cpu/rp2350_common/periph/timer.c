/*
 * SPDX-FileCopyrightText: 2026 Mert Cakir
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rp2350
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Timer (TIMER0) implementation for the RP2350
 *
 * The RP2350 has a 64-bit microsecond timer driven by a 1 MHz tick from the
 * TICKS block (set up at 1 us by the boot ROM). Alarms compare against the
 * low 32 bits of the counter, so the alarm window is a 32-bit wraparound
 * (~71 minutes at 1 MHz).
 *
 * @author      Mert Cakir
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "compat_layer.h"
#include "irq.h"
#include "periph_cpu.h"
#include "periph/timer.h"

#include "regs/timer.h"

#define DEV(d)          (timer_config[d].dev)
#define ALARM(d, a)     ((&(DEV(d)->ALARM0)) + (a))

static timer_cb_t   _ctx_cb[TIMER_NUMOF];
static void        *_ctx_arg[TIMER_NUMOF];
static unsigned     _flag_periodic[TIMER_NUMOF];
static unsigned     _flag_reset[TIMER_NUMOF];

static inline void _timer_reset_count(tim_t dev)
{
    /* Always write TIMELW before TIMEHW; writes only commit when TIMEHW
     * is written (see RP2350 datasheet section 12.8.2 Counter). */
    unsigned state = irq_disable();
    DEV(dev)->TIMELW = 0;
    DEV(dev)->TIMEHW = 0;
    irq_restore(state);
}

static inline bool _is_periodic(tim_t dev, int channel)
{
    return !!(_flag_periodic[dev] & (1U << channel));
}

static inline bool _reset_on_match(tim_t dev, int channel)
{
    return !!(_flag_reset[dev] & (1U << channel));
}

static void _isr(tim_t dev, int channel)
{
    /* INTR is W1C: writing 1 clears the latched interrupt for that alarm */
    DEV(dev)->INTR = 1U << channel;

    if (_is_periodic(dev, channel)) {
        if (_reset_on_match(dev, channel)) {
            _timer_reset_count(dev);
        }
        /* Re-arm by re-writing the same compare value */
        *ALARM(dev, channel) = *ALARM(dev, channel);
    }

    if (_ctx_cb[dev]) {
        _ctx_cb[dev](_ctx_arg[dev], channel);
    }

    rp_end_isr();
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    /* Currently only TIMER0 is exposed (TIMER_NUMOF == 1). It is driven
     * by the 1 us tick from the TICKS block, so 1 MHz is its only
     * supported frequency. A future TIMER1 implementation may relax
     * this check; for now, gate it on dev == 0. */
    if (dev == 0 && freq != 1000000U) {
        return -1;
    }

    _ctx_cb[dev] = cb;
    _ctx_arg[dev] = arg;
    _flag_periodic[dev] = 0;
    _flag_reset[dev] = 0;

    /* Disarm any leftover alarms, mask all alarm interrupts, clear pending */
    DEV(dev)->ARMED = TIMER_ARMED_BITS;          /* W1C: write 1 to disarm */
    atomic_clear(&DEV(dev)->INTE, TIMER_INTE_BITS);
    DEV(dev)->INTR = TIMER_INTR_BITS;            /* W1C: clear all latched IRQs */

    /* Make sure the timer is running */
    DEV(dev)->PAUSE = 0;

    /* Enable each per-channel NVIC line and unmask the alarm in INTE */
    for (uint8_t i = 0; i < timer_config[dev].ch_numof; i++) {
        rp_irq_enable(timer_config[dev].ch[i].irqn);
        atomic_set(&DEV(dev)->INTE, 1U << i);
    }

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    if (channel < 0 || channel >= timer_config[dev].ch_numof) {
        return -EINVAL;
    }

    if (!timeout) {
        /* Avoid the corner case where a tick happens between sampling
         * TIMERAWL and writing the alarm (would arm a full 32-bit period
         * away). Invoke the callback directly. */
        if (_ctx_cb[dev]) {
            _ctx_cb[dev](_ctx_arg[dev], channel);
        }
        return 0;
    }

    unsigned state = irq_disable();
    _flag_periodic[dev] &= ~(1U << channel);
    /* Alarm match is on the low 32 bits of the 64-bit counter */
    uint32_t target = DEV(dev)->TIMERAWL + timeout;
    *ALARM(dev, channel) = target;     /* writing ALARM_x arms the alarm */
    irq_restore(state);
    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    if (channel < 0 || channel >= timer_config[dev].ch_numof) {
        return -EINVAL;
    }

    unsigned state = irq_disable();
    _flag_periodic[dev] &= ~(1U << channel);
    *ALARM(dev, channel) = (uint32_t)value;
    irq_restore(state);
    return 0;
}

int timer_set_periodic(tim_t dev, int channel, unsigned int value, uint8_t flags)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    if (channel < 0 || channel >= timer_config[dev].ch_numof) {
        return -EINVAL;
    }

    if (flags & TIM_FLAG_SET_STOPPED) {
        timer_stop(dev);
    }
    if (flags & TIM_FLAG_RESET_ON_SET) {
        _timer_reset_count(dev);
    }

    unsigned state = irq_disable();
    _flag_periodic[dev] |= (1U << channel);
    if (flags & TIM_FLAG_RESET_ON_MATCH) {
        _flag_reset[dev] |= (1U << channel);
    } else {
        _flag_reset[dev] &= ~(1U << channel);
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
    if (channel < 0 || channel >= timer_config[dev].ch_numof) {
        return -EINVAL;
    }

    /* ARMED is W1C: write 1 to disarm the channel */
    DEV(dev)->ARMED = 1U << channel;
    unsigned state = irq_disable();
    _flag_periodic[dev] &= ~(1U << channel);
    irq_restore(state);
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    /* TIMERAWL is the raw low 32 bits with no latching */
    return DEV(dev)->TIMERAWL;
}

void timer_start(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    DEV(dev)->PAUSE = 0;
}

void timer_stop(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    DEV(dev)->PAUSE = 1;
}

/* RP2350 TIMER0 has four independent IRQ lines (one per alarm channel).
 * The two integer args to _isr() are runtime function-call indices,
 * not register bit positions: the first is the tim_t device index
 * (always 0 here since only TIMER0 is exposed, TIMER_NUMOF == 1), and
 * the second is the alarm channel index 0..3 (mapping 1:1 onto the
 * hardware's ALARM0..ALARM3 registers). Same pattern as the rpx0xx
 * (RP2040) timer driver. */

/* timer 0 IRQ0 */
void TIMER_0_ISRA(void)
{
    _isr(0, 0);
}

/* timer 0 IRQ1 */
void TIMER_0_ISRB(void)
{
    _isr(0, 1);
}

/* timer 0 IRQ2 */
void TIMER_0_ISRC(void)
{
    _isr(0, 2);
}

/* timer 0 IRQ3 */
void TIMER_0_ISRD(void)
{
    _isr(0, 3);
}
