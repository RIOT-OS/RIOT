/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include <errno.h>

#include "auto_init.h"
#include "auto_init_utils.h"
#include "auto_init_priorities.h"

#include "periph/rtc.h"
#include "rtc_utils.h"
#include "walltime.h"
#include "ztimer.h"

#ifndef AUTO_INIT_PRIO_MOD_WALLTIME
#define AUTO_INIT_PRIO_MOD_WALLTIME    AUTO_INIT_PRIO_WDT_EVENT
#endif

static uint32_t _boottime;
#ifdef BACKUP_RAM
static uint32_t _boottime_bkup BACKUP_RAM;
#endif

static void *subscriber;

void walltime_change_subscribe(walltime_change_sub_t *sub)
{
    assert(sub->cb);
    sub->next = NULL;

    void **tail = &subscriber;
    while (*tail) {
        tail = &((walltime_change_sub_t *)*tail)->next;
    }
    *tail = sub;
}

bool walltime_change_unsubscribe(walltime_change_sub_t *sub)
{
    void **tail = &subscriber;
    void **prev = tail;
    while (*tail) {
        if (*tail == sub) {
            *prev = sub->next;
            return true;
        }
        prev = tail;
        tail = &((walltime_change_sub_t *)*tail)->next;
    }

    return false;
}

uint32_t walltime_get_riot(uint16_t *ms)
{
    struct tm now;
    walltime_get(&now, ms);
    return rtc_mktime(&now);
}

time_t walltime_get_unix(uint16_t *ms)
{
    struct tm now;
    walltime_get(&now, ms);
    return mktime(&now);
}

int walltime_set(struct tm *time)
{
    uint32_t now = rtc_mktime(time);
    uint32_t old = walltime_get_riot(NULL);
    int32_t diff = now - old;

    if (now > old && diff <= 0) {
        return -ERANGE;
    }
    if (now < old && diff >= 0) {
        return -ERANGE;
    }

    _boottime += diff;
#ifdef BACKUP_RAM
    _boottime_bkup += diff;
#endif

    int res = walltime_impl_set(time);

    walltime_change_sub_t *tail = subscriber;
    while (tail) {
        tail->cb(tail->ctx, diff, 0);
        tail = tail->next;
    }

    return res;
}

int walltime_get(struct tm *time, uint16_t *ms)
{
    uint16_t msec = 0;
    int res = walltime_impl_get(time, &msec);
    if (ms) {
        *ms = msec;
    }

    return res;
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
    walltime_impl_init();

    _boottime = walltime_get_riot(NULL);
#ifdef BACKUP_RAM
    if (!cpu_woke_from_backup()) {
        _boottime_bkup = _boottime;
    }
#endif
}
AUTO_INIT(auto_init_uptime, AUTO_INIT_PRIO_MOD_WALLTIME);
