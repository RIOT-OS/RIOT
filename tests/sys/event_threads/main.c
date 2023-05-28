/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
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
 * @brief       Event threads test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "event/thread.h"

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
    puts("low");
}

static event_t event_low = { .handler=_handler_low };

int main(void)
{
    event_post(EVENT_PRIO_LOWEST, &event_low);

    puts("main done");

    return 0;
}
