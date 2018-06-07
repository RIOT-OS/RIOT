/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_xtimer
 * @{
 *
 * @file
 * @brief   xtimer hardware abstraction for periph/timer
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include "xtimer.h"

#include "periph/timer.h"

/* access to low-level through generic layer */
#define LLDEV       (*(xtimer_lldev_timer_t *)(dev->ll))
#define LLPARAMS    (*(xtimer_llparams_timer_t *)(dev->params->ll))

static int _dev_init(xtimer_hal_t *dev, xtimer_hal_cb_t cb, void *arg)
{
    timer_init(LLPARAMS.devid, dev->params->hz, cb, *arg);

    timer_set_absolute(LLPARAMS.devid, LLPARAMS.chan,
        _xtimer_lltimer_mask(dev->params->width, (uint32_t)(-1)));
}

static void _dev_set(xtimer_hal_t *dev, uint32_t target,
                     xtimer_hal_cb_t cb, void *arg)
{
    /* callbacks are set in init for this device */
    (void)cb;
    (void)arg;

    timer_set_absolute(LLPARAMS.devid, LLPARAMS.chan,
                       _xtimer_lltimer_mask(dev->params->width, target));
}

static uint32_t _dev_now(xtimer_hal_t *dev)
{
    return timer_read(LLPARAMS.devid);
}

static void _dev_start(xtimer_hal_t *dev)
{
    timer_start(LLPARAMS.devid);
}

static void _dev_stop(xtimer_hal_t *dev)
{
    timer_stop(LLPARAMS.devid);
}

const xtimer_driver_t xtimer_driver_timer = {
    .init = _dev_init,
    .set = _dev_set,
    .now = _dev_now,
    .start = _dev_start,
    .stop = _dev_stop,
};
