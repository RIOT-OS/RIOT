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
 * @brief   xtimer hardware abstraction for periph/rtc
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <time.h>

#include "xtimer.h"

#include "periph/rtc.h"

static int _dev_init(xtimer_hal_t *dev, xtimer_hal_cb_t cb, void *arg)
{
    /* don't rtc_init here, since it is done in drivers/periph_common */
    (void)info;
    (void)cb;
    (void)arg;
}

static void _dev_set(xtimer_hal_t *dev, uint32_t target,
                     xtimer_hal_cb_t cb, void *arg)
{
    struct tm time;

    gmtime_r(_xtimer_lltimer_mask(dev->params->width, target), &time);

    rtc_set_alarm(&time, cb, arg);
}

static uint32_t _dev_now(xtimer_hal_t *dev)
{
    (void)dev;

    struct tm time;

    rtc_get_time(&time);

    return (uint32_t)mktime(&time);
}

static void _dev_start(xtimer_hal_t *dev)
{
    (void)dev;

    rtc_poweron();
}

static void _dev_stop(xtimer_hal_t *dev)
{
    (void)dev;

    rtc_poweroff();
}

const xtimer_driver_t xtimer_driver_rtc = {
    .init = _dev_init,
    .set = _dev_set,
    .now = _dev_now,
    .start = _dev_start,
    .stop = _dev_stop,
};
