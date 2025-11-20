/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Periodic Callback test application
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "event/deferred_callback.h"
#include "event/periodic_callback.h"
#include "event/thread.h"

static void _event_cb(void *ctx)
{
    puts(ctx);
}

int main(void)
{
    event_callback_t oneshot;
    event_callback_oneshot(&oneshot, EVENT_PRIO_MEDIUM, _event_cb, "event 0");

    event_deferred_callback_t d;
    event_deferred_callback_post(&d, EVENT_PRIO_MEDIUM, ZTIMER_MSEC, 750, _event_cb, "event D");

    event_periodic_callback_t a, b, c;
    event_periodic_callback_init(&a, ZTIMER_MSEC, EVENT_PRIO_MEDIUM, _event_cb, "event A");
    event_periodic_callback_init(&b, ZTIMER_MSEC, EVENT_PRIO_MEDIUM, _event_cb, "event B");
    event_periodic_callback_init(&c, ZTIMER_MSEC, EVENT_PRIO_MEDIUM, _event_cb, "event C");

    event_periodic_callback_set_count(&a, 6);
    event_periodic_callback_set_count(&b, 3);
    event_periodic_callback_set_count(&c, 2);

    event_periodic_callback_start(&a,  502);
    event_periodic_callback_start(&b, 1001);
    event_periodic_callback_start(&c, 1500);

    ztimer_sleep(ZTIMER_MSEC, 3020);

    return 0;
}
