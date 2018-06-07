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
 * @brief   xtimer hardware abstraction for periph/rtt
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include "xtimer.h"

#include "periph/rtt.h"

static int _dev_init(xtimer_hal_t *dev, xtimer_hal_cb_t cb, void *arg)
{
    (void)dev;

    rtt_init();

    rtt_set_overflow_cb(cb, *arg);
}

static void _dev_set(xtimer_hal_t *dev, uint32_t target,
                     xtimer_hal_cb_t cb, void *arg)
{
    rtt_set_alarm(_xtimer_lltimer_mask(dev->params.width, target), cb, arg);
}

static uint32_t _dev_now(xtimer_hal_t *dev)
{
    (void)dev;

    return rtt_get_counter;
}

static void _dev_start(xtimer_hal_t *dev)
{
    (void)dev;

    rtt_poweron();
}

static void _dev_stop(xtimer_hal_t *dev)
{
    (void)dev;

    rtt_poweroff();
}

const xtimer_driver_t xtimer_driver_rtt = {
    .init = _dev_init,
    .set = _dev_set,
    .now = _dev_now,
    .start = _dev_start,
    .stop = _dev_stop,
};
