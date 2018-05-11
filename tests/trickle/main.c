/*
 * Copyright (C) 2017 HAW Hamburg
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
 * @brief Trickle test application
 *
 * @author Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "trickle.h"
#include "thread.h"
#include "msg.h"

#define TRICKLE_MSG     (0xfeef)
#define TR_IMIN         (8)
#define TR_IDOUBLINGS   (20)
#define TR_REDCONST     (10)
#define FIRST_ROUND     (5)
#define SECOND_ROUND    (12)

static uint32_t prev_now = 0, prev_diff = 0;
static bool error = false;

static void callback(void *args)
{
    (void) args;
    uint32_t now = xtimer_now_usec();
    uint32_t diff = (uint32_t) (now - prev_now);

    printf("now = %" PRIu32 ", prev_now = %" PRIu32 ", diff = %" PRIu32
           "\n", now, prev_now, diff);

    if (prev_diff >= diff) {
        error = true;
    }

    prev_now = now;
    prev_diff = diff;

    return;
}

static trickle_t trickle = { .callback = { .func = &callback,
                                           .args = NULL } };

int main(void)
{
    msg_t msg;
    unsigned counter = 0;
    prev_now = xtimer_now_usec();

    trickle_start(sched_active_pid, &trickle, TRICKLE_MSG, TR_IMIN,
                  TR_IDOUBLINGS, TR_REDCONST);

    puts("[START]");

    while (!error) {
        if (counter == FIRST_ROUND) {
            prev_diff = 0;
            trickle_reset_timer(&trickle);
            puts("[TRICKLE_RESET]");
        }
        else if (counter == SECOND_ROUND) {
            puts("[SUCCESS]");
            return 0;
        }

        counter++;

        msg_receive(&msg);

        switch (msg.type) {
            case TRICKLE_MSG:
                trickle_callback((trickle_t *) msg.content.ptr);
                break;
            default:
                break;
        }
    }

    puts("[FAILURE]");

    return 1;
}
