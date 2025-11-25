/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Timer implementation for the RPX0XX
 * @details     The RPX0XX has a 64 bit µs timer but timer interrupts match
 *              on the lower 32 bits.
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include "vendor/RP2040.h"
#include "io_reg.h"
#include "timex.h"
#include "periph_conf.h"
#include "periph/timer.h"

#define DEV(d)          (timer_config[d].dev)
#define ALARM(d, a)     ((&(DEV(d)->ALARM0)) + (a))

static timer_cb_t _timer_ctx_cb[TIMER_NUMOF];
static void *_timer_ctx_arg[TIMER_NUMOF];
static unsigned _timer_flag_periodic[TIMER_NUMOF];
static unsigned _timer_flag_reset[TIMER_NUMOF];

static inline uint64_t _timer_read_us(tim_t dev)
{
    /* This is not safe when the second core also accesses the timer */
    unsigned state = irq_disable();
    uint32_t lo = DEV(dev)->TIMELR; /* always read timelr to latch the value of timehr */
    uint32_t hi = DEV(dev)->TIMEHR; /* read timehr to unlatch */
    irq_restore(state);
    return ((uint64_t)hi << 32U) | lo;
}

static inline void _timer_reset(tim_t dev)
{
    unsigned state = irq_disable();
    DEV(dev)->TIMELW = 0; /* always write timelw before timehw */
    DEV(dev)->TIMEHW = 0; /* writes do not get copied to time until timehw is written */
    irq_restore(state);
}

static inline void _timer_enable_periodic(tim_t dev, int channel, uint8_t flags)
{
    _timer_flag_periodic[dev] |= (1U << channel);
    if (flags & TIM_FLAG_RESET_ON_MATCH) {
        _timer_flag_reset[dev] |= (1U << channel);
    }
    else {
        _timer_flag_reset[dev] &= ~(1U << channel);
    }
}

static inline void _timer_disable_periodic(tim_t dev, int channel)
{
    _timer_flag_periodic[dev] &= ~(1U << channel);
}

static inline bool _timer_is_periodic(tim_t dev, int channel)
{
    return !!(_timer_flag_periodic[dev] & (1U << channel));
}

static inline bool _timer_reset_on_match(tim_t dev, int channel)
{
    return !!(_timer_flag_reset[dev] & (1U << channel));
}

static inline void _irq_enable(tim_t dev)
{
    for (uint8_t i = 0; i < timer_config[dev].ch_numof; i++) {
        NVIC_EnableIRQ(timer_config[dev].ch[i].irqn);
        io_reg_atomic_set(&DEV(dev)->INTE, (1U << i));
    }
}

static void _isr(tim_t dev, int channel)
{
    /* clear latched interrupt */
    io_reg_atomic_clear(&DEV(dev)->INTR, 1U << channel);

    if (_timer_is_periodic(dev, channel)) {
        if (_timer_reset_on_match(dev, channel)) {
            _timer_reset(dev);
        }
        /* rearm */
        *ALARM(dev, channel) = *ALARM(dev, channel);
    }
    if (_timer_ctx_cb[dev]) {
        _timer_ctx_cb[dev](_timer_ctx_arg[dev], channel);
    }

    cortexm_isr_end();
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    /* The timer must run at 1000000 Hz (µs precision)
       because the number of cycles per µs is shared with the watchdog.
       The reference clock (clk_ref) is divided by
       (WATCHDOG->TICK & WATCHDOC_TICK_CYCLES_Mask)
       to generate µs ticks.
     */
    assert(freq == US_PER_SEC); (void)freq;
    _timer_ctx_cb[dev] = cb;
    _timer_ctx_arg[dev] = arg;
    periph_reset(RESETS_RESET_timer_Msk);
    periph_reset_done(RESETS_RESET_timer_Msk);
    io_reg_write_dont_corrupt(&WATCHDOG->TICK,
                              (CLOCK_XOSC / MHZ(1)) << WATCHDOG_TICK_CYCLES_Pos,
                              WATCHDOG_TICK_CYCLES_Msk);
    _irq_enable(dev);
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
        /* execute callback immediately if timeout equals 0,
           to ctach the case that a tick happens right before arming the alarm
           and causes a full timer period to elaps */
        if (_timer_ctx_cb[dev]) {
            _timer_ctx_cb[dev](_timer_ctx_arg[dev], channel);
        }
    }
    else {
        unsigned state = irq_disable();
        _timer_disable_periodic(dev, channel);
        /* an alarm interrupt matches on the lower 32 bit of the 64 bit timer counter */
        uint64_t target = DEV(dev)->TIMERAWL + timeout;
        *ALARM(dev, channel) = (uint32_t)target;
        irq_restore(state);
    }
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
    _timer_disable_periodic(dev, channel);
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
        _timer_reset(dev);
    }
    unsigned state = irq_disable();
    _timer_enable_periodic(dev, channel, flags);
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
    /* ARMED bits are write clear */
    io_reg_atomic_set(&DEV(dev)->ARMED, (1 << channel));
    unsigned state = irq_disable();
    _timer_disable_periodic(dev, channel);
    irq_restore(state);
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    if (dev >= TIMER_NUMOF) {
        return -ENODEV;
    }
    return _timer_read_us(dev);
}

void timer_start(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    io_reg_atomic_clear(&DEV(dev)->PAUSE, (1 << TIMER_PAUSE_PAUSE_Pos));
}

void timer_stop(tim_t dev)
{
    assert(dev < TIMER_NUMOF);
    io_reg_atomic_set(&DEV(dev)->PAUSE, (1 << TIMER_PAUSE_PAUSE_Pos));
}

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
