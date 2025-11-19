/*
 * SPDX-FileCopyrightText: 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief IRQ test application
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "thread.h"

static char busy_stack[THREAD_STACKSIZE_MAIN];
static volatile int busy, i, k;

void *busy_thread(void *arg)
{
    (void) arg;

    int j = 0;
    puts("busy_thread starting");

    i = 0;

    while (busy) {
        k = j = ++i;
    }

    printf("i: %i\n", i);
    printf("j: %i\n", j);
    printf("k: %i\n", k);

    puts("SUCCESS");

    return NULL;
}

int main(void)
{
    puts("START");

    busy = 1;
    k = 23;
    thread_create(busy_stack, sizeof(busy_stack),
                  THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_WOUT_YIELD,
                  busy_thread, NULL, "busy_thread");
    puts("busy_thread created");

    puts("xtimer_wait()");
    xtimer_usleep(100000);
    busy = 0;

    puts("main: return");

    return 0;
}
