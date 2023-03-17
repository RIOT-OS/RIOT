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
#include "ztimer.h"


#define TRICKLE_MSG     (0xfeef)
#define TR_IMIN         (8)
#define TR_IDOUBLINGS   (20)
#define TR_REDCONST     (10)
#define FIRST_ROUND     (5)
#define SECOND_ROUND    (12)

#define MAIN_QUEUE_SIZE     (2)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static uint32_t old_t = 0;
static bool error = false;

static void callback(void *args);

static trickle_t trickle = { .callback = { .func = &callback,
                                           .args = NULL } };

static void callback(void *args)
{
    (void) args;
    uint32_t now = ztimer_now(ZTIMER_MSEC);
    printf("now = %" PRIu32 ", t = %" PRIu32 "\n", now, trickle.t);

    /* previous `t` is chosen from a smaller interval [I/2, I).
     * Current `t` is chosen from interval [I, 2*I).
     * Hence, `old_t` must be smaller than current `t` */
    if (old_t >= trickle.t) {
        error = true;
    }

    old_t = trickle.t;

    return;
}

int main(void)
{
    msg_t msg;
    unsigned counter = 0;

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    trickle_start(thread_getpid(), &trickle, TRICKLE_MSG, TR_IMIN,
                  TR_IDOUBLINGS, TR_REDCONST);

    puts("[START]");

    while (!error) {
        if (counter == FIRST_ROUND) {
            old_t = 0;
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
