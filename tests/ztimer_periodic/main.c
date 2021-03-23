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
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       ztimer periodic test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "ztimer.h"
#include "ztimer/periodic.h"
#include "fmt.h"
#include "mutex.h"

static mutex_t _mutex = MUTEX_INIT_LOCKED;

#define MAX_OFFSET 2
#define N 5
#define INTERVAL 100LU

static uint32_t _times[N];

static int callback(void *arg)
{
    (void)arg;
    static int count = 0;

    _times[count] = ztimer_now(ZTIMER_MSEC);

    count += 1;

    /* enable this to test underflow behavior */
#if 0
    if (count == 2) {
        ztimer_spin(ZTIMER_MSEC, INTERVAL*2);
    }
#endif

    if (count == N) {
        mutex_unlock(&_mutex);
    }

    return (count == N);
}

int main(void)
{
    ztimer_periodic_t t;

    ztimer_periodic_init(ZTIMER_MSEC, &t, callback, NULL, INTERVAL);
    uint32_t last = ztimer_now(ZTIMER_MSEC);

    ztimer_periodic_start(&t);

    if (!ztimer_is_set(ZTIMER_MSEC, &t.timer)) {
        print_str("Test failed\n");
        return 1;
    }

    /* wait for periodic to trigger N times */
    mutex_lock(&_mutex);

    int failed = 0;

    for (unsigned i = 0; i < N; i++) {
        uint32_t offset = labs((int32_t)(_times[i] - INTERVAL - last));
        printf("i: %u time: %" PRIu32 " offset: %" PRIu32 "\n",
               i, _times[i], offset);
        if (offset > MAX_OFFSET) {
            failed = 1;
        }
        last = _times[i];
    }

    if (!failed) {
        print_str("Test successful.\n");
    }
    else {
        print_str("Test failed!\n");
    }
}
