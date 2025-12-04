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
 * @brief       Wall-Clock ztimer fallback
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "rtc_utils.h"
#include "walltime.h"
#include "ztimer.h"

static uint32_t ztimer_offset;

__attribute__((weak))
int walltime_impl_get(struct tm *time, uint16_t *ms)
{
    uint32_t now = ztimer_now(ZTIMER_MSEC);
    now = now / 1000 + ztimer_offset;
    *ms = now % 1000;

    rtc_localtime(now, time);

    return 0;
}

__attribute__((weak))
int walltime_impl_set(struct tm *time)
{
    ztimer_offset = rtc_mktime(time) - ztimer_now(ZTIMER_MSEC) / 1000;
    return 0;
}
