/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Event threads test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>

#include "thread.h"
#include "event/thread.h"

static_assert(EVENT_THREAD_MEDIUM_STACKSIZE == 567,
              "Selecting highest of the minimum stack size requirements "
              "declared via `EVENT_THREAD_HIGHEST_STACKSIZE_MIN += <num>` "
              "must work correctly");

static void _handler_high(event_t *event) {
    (void)event;
    puts("high");
}

static event_t event_high = { .handler=_handler_high };

static void _handler_medium(event_t *event) {
    (void)event;
    event_post(EVENT_PRIO_HIGHEST, &event_high);
    puts("medium");
}

static event_t event_medium = { .handler=_handler_medium };

static void _handler_low(event_t *event) {
    (void)event;
    event_post(EVENT_PRIO_MEDIUM, &event_medium);
    event_post(EVENT_PRIO_HIGHEST, &event_high);
    puts("low");
}

static event_t event_low = { .handler=_handler_low };

int main(void)
{
    event_post(EVENT_PRIO_LOWEST, &event_low);

    puts("main done");

    return 0;
}
