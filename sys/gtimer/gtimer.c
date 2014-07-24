/**
 * global timer
 *
 * Copyright (C) 2014 Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup gtimer
 * @{
 * @file
 * @brief   Global clock implementation for clock-sync protocols
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author  Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "timex.h"
#include "msg.h"
#include "gtimer.h"
#include "mutex.h"
#include "vtimer.h"

#define GTIMER_JUMP_WARN_THRESHOLD ((int64_t)1000*1000*1000)

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#define GTIMER_WARNINGS
#endif
#include "debug.h"

gtimer_timeval_t gtimer_last;
mutex_t gtimer_mutex;

static void _gtimer_refresh_last_local(void);

static void _gtimer_now(gtimer_timeval_t *out);

void gtimer_now(timex_t *out)
{
    gtimer_timeval_t now;
    gtimer_sync_now(&now);
    timex_t result = timex_from_uint64(now.global);
    *out = result;
}

void gtimer_init(void)
{
    DEBUG("gtimer_init()\n");
    timex_t now;
    vtimer_now(&now);
    mutex_init(&gtimer_mutex);
    gtimer_last.local = timex_uint64(now);
    gtimer_last.global = timex_uint64(now);
    gtimer_last.rate = 0.0;
}

void gtimer_sync_now(gtimer_timeval_t *out)
{
    mutex_lock(&gtimer_mutex);
    _gtimer_now(out);
    mutex_unlock(&gtimer_mutex);
}

void gtimer_sync_set_global_offset(int64_t global_offset)
{
    mutex_lock(&gtimer_mutex);
    _gtimer_refresh_last_local();
#ifdef GTIMER_WARNINGS
    if (global_offset < 0)
    {
        puts("gtimer_sync_set_global_offset: warning: offset negative!");
    }
    if (global_offset > GTIMER_JUMP_WARN_THRESHOLD)
    {
        puts(
                "gtimer_sync_set_global_offset: warning: jumping more than 10^9 us in the future!");
    }
#endif /* GTIMER_WARNINGS */
    int64_t new_global = (int64_t) gtimer_last.global + (int64_t) global_offset;
    if (new_global < 0)
    {
        gtimer_last.global = 0;
    }
    else
    {
        gtimer_last.global = (uint64_t) new_global;
    }
    mutex_unlock(&gtimer_mutex);
}

void gtimer_sync_set_relative_rate(float rate)
{
    mutex_lock(&gtimer_mutex);
    _gtimer_refresh_last_local();
#ifdef GTIMER_WARNINGS
    if (rate > 1 || rate < -1)
        puts("gtimer_sync_set_relative_rate: rate to large!");
#endif /* GTIMER_WARNINGS */

    gtimer_last.rate = rate;
    mutex_unlock(&gtimer_mutex);
}

float gtimer_sync_get_relative_rate(void)
{
    return gtimer_last.rate;
}

static void _gtimer_now(gtimer_timeval_t *out)
{
    uint64_t now;
    timex_t temp;
    vtimer_now(&temp);
    now = timex_uint64(temp);
    DEBUG("clockrate multiply: %"PRIu64 "\n", ((now - gtimer_last.local) * gtimer_last.rate));
    out->global = gtimer_last.global
            + (((now - gtimer_last.local) * gtimer_last.rate)
                    + (now - gtimer_last.local));
    out->local = now;
    out->rate = gtimer_last.rate;
}

static void _gtimer_refresh_last_local(void)
{
    gtimer_timeval_t now;
    _gtimer_now(&now);
    gtimer_last = now;
}
