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
 * @brief       Watchdog Thread
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "auto_init.h"
#include "auto_init_utils.h"
#include "auto_init_priorities.h"

#include "architecture.h"
#include "periph/wdt.h"
#include "wdt_thread.h"
#include "ztimer.h"

#ifndef WDT_THREAD_STACKSIZE
#if DEVELHELP
#define WDT_THREAD_STACKSIZE    THREAD_STACKSIZE_SMALL
#else
#define WDT_THREAD_STACKSIZE    THREAD_STACKSIZE_TINY
#endif
#endif

static char WORD_ALIGNED wdt_stack[WDT_THREAD_STACKSIZE];

static bool _wdt_enabled = true;

void wdt_thread_stop(void)
{
    _wdt_enabled = false;
    wdt_stop();
}

static void *_wdt_thread(void *ctx)
{
    (void)ctx;
    unsigned sleep_ms = (CONFIG_PERIPH_WDT_WIN_MIN_MS + CONFIG_PERIPH_WDT_WIN_MAX_MS)
                      / 2;
    while (1) {
        ztimer_sleep(ZTIMER_MSEC, sleep_ms);
        if (!_wdt_enabled) {
            break;
        }
        wdt_kick();
    }

    return NULL;
}

static void auto_init_wdt_thread(void)
{
    thread_create(wdt_stack, sizeof(wdt_stack), THREAD_PRIORITY_MIN,
                  0, _wdt_thread, NULL, "watchdog");
}

AUTO_INIT(auto_init_wdt_thread, AUTO_INIT_PRIO_WDT_THREAD);
