/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2017 Eistec AB
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
 * @brief       timer test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"
#include "msg.h"
#include "xtimer.h"

#ifndef NUMOF
#define NUMOF   (256)
#endif

int32_t res[NUMOF];

int main(void)
{
    puts("xtimer_periodic_msg test application.\n");
    msg_t msg_q[1];
    msg_init_queue(msg_q, 1);

    uint32_t interval = NUMOF;
    int32_t max_diff = INT32_MIN;
    int32_t min_diff = INT32_MAX;

    for (int i = 0; i < NUMOF; i++) {
        xtimer_ticks32_t now = xtimer_now();
        printf("Testing interval %" PRIu32 "... (now=%" PRIu32 ")\n", interval, xtimer_usec_from_ticks(now));
        xtimer_ticks32_t last_wakeup = xtimer_now();
        xtimer_ticks32_t before = last_wakeup;
        xtimer_t timer = { .target = 0, .long_target = 0 };
        msg_t msg_out = { .type = 0xffff - interval, .content.value = interval };
        xtimer_periodic_msg(&timer, &last_wakeup, interval, &msg_out, thread_getpid());
        msg_t msg_in;
        msg_receive(&msg_in);
        now = xtimer_now();
        assert(msg_in.type == msg_out.type);
        assert(msg_in.content.value == msg_out.content.value);
        res[i] = (xtimer_usec_from_ticks(now) - xtimer_usec_from_ticks(before)) - interval;
        interval -= 1;
    }

    for (int i = 0; i < NUMOF; i++) {
        printf("%4d diff=%" PRIi32 "\n", NUMOF - i, res[i]);

        if (res[i] > max_diff) {
            max_diff = res[i];
        }
        if (res[i] < min_diff) {
            min_diff = res[i];
        }
    }
    printf("\nMin/max error: %" PRId32 "/%" PRId32 "\n", min_diff, max_diff);

    if (min_diff < -1000 || max_diff > 1000) {
        puts("too large difference.\n");
        puts("Test Failed.\n");
        return 1;
    }

    printf("\nTest complete.\n");
    return 0;
}
