/*
 * Copyright (C) 2021 Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_countdown
 * @{
 *
 * @file
 * @brief       Shared countdown timer code
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdlib.h>

#define _PERIPH_COUNTDOWN_XFA_INIT
#include "periph/countdown.h"

#define COUNTDOWN_DEV(i)    &countdown_timers_xfa[i]

XFA_INIT_CONST(countdown_dev_t, countdown_timers_xfa);
XFA_INIT(countdown_cfg_t, countdown_cfg_xfa);
#if IS_USED(MODULE_PERIPH_COUNTDOWN_LONG)
#define COUNTDOWN_EXTEND 1
#else
#define COUNTDOWN_EXTEND 0
#endif

/**
 * @brief Shift frequency up to preserve accuracy when dividing
 */
#define FREQ_SHIFT  (8)

static const uint32_t unit_mul[] = {
    1, MS_PER_SEC, US_PER_SEC, NS_PER_SEC
};

static uint8_t _get_base_unit(uint32_t freq_hz, uint32_t *ticks_per_unit)
{
    if (freq_hz >= US_PER_SEC) {
        *ticks_per_unit = ((uint64_t)freq_hz << FREQ_SHIFT) / US_PER_SEC;
        return COUNTDOWN_FLAG_UNIT_US;
    }

    if (freq_hz >= MS_PER_SEC) {
        *ticks_per_unit = (freq_hz << FREQ_SHIFT) / MS_PER_SEC;
        return COUNTDOWN_FLAG_UNIT_MS;
    }

    if (freq_hz >= 1) {
        *ticks_per_unit = freq_hz << FREQ_SHIFT;
        return COUNTDOWN_FLAG_UNIT_SEC;
    }

    assert(0);
    return 0;
}

bool countdown_init(countdown_t idx, uint32_t max_range, unsigned flags,
                    countdown_cb_t cb, void *arg)
{
    uint32_t freq, max_timeout, mask;
    const volatile countdown_dev_t *dev = COUNTDOWN_DEV(idx);

    freq = dev->driver->init(dev->ctx, max_range, flags, &mask);
    dev->driver->set_cb(dev->ctx, cb, arg);

    countdown_cfg_xfa[idx].base_unit = _get_base_unit(freq,
                                                      &countdown_cfg_xfa[idx].ticks_per_unit);
    max_timeout = mask - mask % (countdown_cfg_xfa[idx].ticks_per_unit >> FREQ_SHIFT);
#if COUNTDOWN_EXTEND
    countdown_cfg_xfa[idx].max_timeout = max_timeout;
    countdown_cfg_xfa[idx].real_cb = NULL;
#endif

    return (uint64_t)max_timeout * unit_mul[countdown_cfg_xfa[idx].base_unit]
        >= (uint64_t)max_range * unit_mul[flags & COUNTDOWN_FLAG_UNIT_MASK];
}

#if COUNTDOWN_EXTEND
static uint64_t _get_timeout64(countdown_cfg_t *cfg, uint64_t timeout, unsigned flags)
{
    uint8_t unit = flags & COUNTDOWN_FLAG_UNIT_MASK;

    if (unit >= cfg->base_unit) {
        uint8_t diff = unit - cfg->base_unit;
        timeout = timeout * cfg->ticks_per_unit * unit_mul[diff];
    }
    else {
        uint8_t diff = cfg->base_unit - unit;
        timeout = timeout * cfg->ticks_per_unit / unit_mul[diff];
    }

    return timeout >> FREQ_SHIFT;
}
#endif

static void _set_alarm(const volatile countdown_dev_t *dev, countdown_cfg_t *cfg,
                       uint32_t timeout, unsigned flags)
{
    bool one_shot = !(flags & COUNTDOWN_FLAG_PERIODIC);
    uint8_t unit  = flags & COUNTDOWN_FLAG_UNIT_MASK;

    if (unit >= cfg->base_unit) {
        uint8_t diff = unit - cfg->base_unit;
        timeout = timeout * cfg->ticks_per_unit * unit_mul[diff];
    }
    else {
        uint8_t diff = cfg->base_unit - unit;
        timeout = timeout * cfg->ticks_per_unit / unit_mul[diff];
    }
    timeout >>= FREQ_SHIFT;

    dev->driver->set(dev->ctx, timeout, one_shot);
}

void countdown_set(countdown_t idx, uint32_t timeout, unsigned flags)
{
    const volatile countdown_dev_t *dev = COUNTDOWN_DEV(idx);
    countdown_cfg_t *cfg = &countdown_cfg_xfa[idx];

#if COUNTDOWN_EXTEND
    /* make sure we don't use the timer extend cb */
    if (cfg->real_cb) {
        dev->driver->cancel(dev->ctx);
        dev->driver->set_cb(dev->ctx, cfg->real_cb, cfg->real_ctx);
        cfg->real_cb = NULL;
    }
#endif

    _set_alarm(dev, cfg, timeout, flags);
}

#if COUNTDOWN_EXTEND
static inline void _swap(uint32_t *a, uint32_t *b)
{
    uint32_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static void _extend_cb(void *ctx)
{
    unsigned idx = (uintptr_t)ctx;
    const volatile countdown_dev_t *dev = COUNTDOWN_DEV(idx);
    countdown_cfg_t *cfg = &countdown_cfg_xfa[idx];

    /* timer is periodic if there is a reload value */
    bool one_shot = cfg->skip_cb[1] == 0;

    switch (--cfg->skip_cb[0]) {
    case 1:
        /* set remaining time */
        dev->driver->set(dev->ctx, cfg->remaining, one_shot);
        dev->driver->start(dev->ctx);
        break;
    case 0:
        /* reload periodic timer if needed */
        if (!one_shot) {
            cfg->skip_cb[0] = cfg->skip_cb[1];

            dev->driver->set(dev->ctx, cfg->max_timeout, one_shot);
            dev->driver->start(dev->ctx);
        }

        /* execute callback */
        cfg->real_cb(cfg->real_ctx);
        break;
    }
}

void countdown_set_long(countdown_t idx, uint32_t timeout, unsigned flags)
{
    const volatile countdown_dev_t *dev = COUNTDOWN_DEV(idx);
    countdown_cfg_t *cfg = &countdown_cfg_xfa[idx];
    lldiv_t d;

    uint64_t timeout64 = _get_timeout64(cfg, timeout, flags);

    /* set normal countdown if the timeout fits in a single range */
    if (timeout64 <= cfg->max_timeout) {
        countdown_set(idx, timeout, flags);
        return;
    }

    /* first stop the timer */
    dev->driver->cancel(dev->ctx);

    d = lldiv(timeout64, cfg->max_timeout);
    cfg->skip_cb[0] = d.quot + 1;
    cfg->remaining = d.rem;

    /* add reload value for periodic execution */
    if (flags & COUNTDOWN_FLAG_PERIODIC) {
        cfg->skip_cb[1] = cfg->skip_cb[0];
    }
    else {
        cfg->skip_cb[1] = 0;
    }

    /* swap out callback */
    if (cfg->real_cb == NULL) {
        dev->driver->get_cb(dev->ctx, &cfg->real_cb, &cfg->real_ctx);
        dev->driver->set_cb(dev->ctx, _extend_cb, (void*)(uintptr_t)idx);
    }

    dev->driver->set(dev->ctx, cfg->max_timeout, false);
}
#endif /* COUNTDOWN_EXTEND */

void countdown_start(countdown_t idx)
{
    const volatile countdown_dev_t *dev = COUNTDOWN_DEV(idx);
    dev->driver->start(dev->ctx);
}

void countdown_cancel(countdown_t idx)
{
    const volatile countdown_dev_t *dev = COUNTDOWN_DEV(idx);
    dev->driver->cancel(dev->ctx);
}
