/*
 * Copyright (C) 2021 TUBA Freiberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief    high level ztimer test application
 *
 * @author   Karl Fessel <karl.fessel@ovgu.de>
 *
 *
 * @}
 */

#include <stdio.h>

#include "ztimer.h"
#include "mutex.h"

/* only header information is used we do not need to use MODULE_TIMEX */
#include "timex.h"

#define INTERVAL 1000

void track(void *arg);

typedef struct ztimer_tracker {ztimer_t base; uint64_t exp;} ztimer_tracker_t;

static ztimer_tracker_t usec_tracker = { .base.callback = track,
                                         .base.arg = &usec_tracker,
                                         .exp = 0 };

void track(void *arg)
{
    ztimer_tracker_t *e = arg;

    e->exp += INTERVAL * US_PER_MS;

    ztimer_set(ZTIMER_USEC, &(e->base), INTERVAL * US_PER_MS);
}

uint64_t track_time(void){
    return usec_tracker.exp - ztimer_until(ZTIMER_USEC, &(usec_tracker.base));
}

int main(void)
{
    puts("starting ztimers");
    /* a high level ztimer*/
    if (0) { /* 0 is now*/
        ztimer_tracker_t *e = & usec_tracker;
        e->exp += INTERVAL * US_PER_MS;

        ztimer_set(ZTIMER_USEC, &(e->base), INTERVAL * US_PER_MS);
    }

    if (0) { /* 0 is yesterday*/
        ztimer_tracker_t *e = & usec_tracker;
        /*need to ensure these are 64Bit -> cast one element to uint64_t*/
        e->exp = (INTERVAL * US_PER_MS) + ((uint64_t)24 * 60 * 60 * US_PER_SEC);

        ztimer_set(ZTIMER_USEC, &(e->base), INTERVAL * US_PER_MS);
    }

    { /* 0 is yesteryear*/
        ztimer_tracker_t *e = & usec_tracker;
        /*need to ensure these are 64Bit -> cast one element to uint64_t*/
        e->exp = (INTERVAL * US_PER_MS) + ((uint64_t)365 * 24 * 60 * 60 * US_PER_SEC);

        ztimer_set(ZTIMER_USEC, &(e->base), INTERVAL * US_PER_MS);
    }

    while (ztimer_spin(ZTIMER_USEC, 300* US_PER_MS), 1) {
        uint64_t t = track_time();
        printf( "time now:\t%" PRIu32 "\t time track:\t%" PRIu64
                "\t upper32:\t%" PRIu32 "\t lower32:\t%" PRIu32 "\n",
                (uint32_t)ztimer_now(ZTIMER_USEC),
                t, (uint32_t)(t>>32), (uint32_t)(t));
    }
    printf("SUCCESS!\n");
    return 0;
}
