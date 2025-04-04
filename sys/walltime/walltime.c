/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_walltime
 * @{
 *
 * @file
 * @brief       Wall-Clock time access
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */
#include "auto_init.h"
#include "auto_init_utils.h"
#include "auto_init_priorities.h"

#include "periph/rtc.h"
#include "rtc_utils.h"
#include "walltime.h"
#include "ztimer.h"

#if !defined(MODULE_PERIPH_RTC) && !defined(MODULE_RTT_RTC)
#define ZTIMER_FALLBACK
#endif

static uint32_t _boottime;
#ifdef BACKUP_RAM
static uint32_t _boottime_bkup;
#endif
#ifdef ZTIMER_FALLBACK
uint32_t ztimer_offset;
#endif

uint32_t walltime_get_riot(uint16_t *ms)
{
#ifdef ZTIMER_FALLBACK
    uint32_t now = ztimer_now(ZTIMER_MSEC);
    if (ms) {
        *ms = now % 1000;
    }
    return now / 1000 + ztimer_offset;
#else
    struct tm now;
    walltime_get(&now, ms);
    return rtc_mktime(&now);
#endif
}

time_t walltime_get_unix(uint16_t *ms)
{
    struct tm now;
    walltime_get(&now, ms);
    return mktime(&now);
}

void walltime_set(struct tm *time)
{
    int32_t diff = rtc_mktime(time) - walltime_get_riot(NULL);
    _boottime += diff;
#ifdef BACKUP_RAM
    _boottime_bkup += diff;
#endif

#ifdef ZTIMER_FALLBACK
    ztimer_offset = rtc_mktime(time) - ztimer_now(ZTIMER_MSEC)/1000;
#else
    rtc_set_time(time);
#endif
}

void walltime_get(struct tm *time, uint16_t *ms)
{
    uint32_t msec = 0;
#ifdef ZTIMER_FALLBACK
    rtc_localtime(walltime_get_riot(ms), time);
#elif defined(MODULE_PERIPH_RTC_MS)
    rtc_get_time_ms(time, &msec);
#else
    rtc_get_time(time);
#endif
    if (ms) {
        *ms = msec;
    }
}

uint32_t walltime_uptime(bool full)
{
    uint32_t now = walltime_get_riot(NULL);
#ifdef BACKUP_RAM
    if (full) {
        return now - _boottime_bkup;
    }
#else
    (void)full;
#endif
    return now - _boottime;
}

static void auto_init_uptime(void)
{
    _boottime = walltime_get_riot(NULL);
#ifdef BACKUP_RAM
    if (!cpu_woke_from_backup()) {
        _boottime_bkup = _boottime;
    }
#endif
}
AUTO_INIT(auto_init_uptime, AUTO_INIT_PRIO_WDT_EVENT);
