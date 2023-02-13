/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 *
 * @file        wdt.c
 * @brief       Watchdog Event
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "auto_init.h"
#include "auto_init_utils.h"
#include "auto_init_priorities.h"

#include "architecture.h"
#include "event/periodic_callback.h"
#include "event/thread.h"
#include "periph/wdt.h"
#include "ztimer.h"

static void _wdt_event_cb(void *ctx)
{
    (void)ctx;
    wdt_kick();
}

static void auto_init_wdt_event(void)
{
    static event_periodic_callback_t wdt_event;
    unsigned sleep_ms = (CONFIG_PERIPH_WDT_WIN_MIN_MS + CONFIG_PERIPH_WDT_WIN_MAX_MS)
                      / 2;

    event_periodic_callback_init(&wdt_event, ZTIMER_MSEC, EVENT_PRIO_LOWEST, _wdt_event_cb, NULL);
    event_periodic_callback_start(&wdt_event, sleep_ms);
}

AUTO_INIT(auto_init_wdt_event, AUTO_INIT_PRIO_WDT_EVENT);
