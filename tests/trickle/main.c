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

#define Q_LEN           (8)
#define TRICKLE_MSG     (0xfeef)
#define TR_IMIN         (8)
#define TR_IDOUBLINGS   (20)
#define TR_REDCONST     (10)

static msg_t _msg_q[Q_LEN];

static void callback(void *args)
{
    (void) args;
    printf("now: %" PRIu32 "\n", xtimer_now_usec());
    return;
}

static trickle_t trickle = { .callback.func = &callback,
                             .callback.args = NULL };

int main(void)
{
    msg_t msg;

    msg_init_queue(_msg_q, Q_LEN);

    trickle_start(sched_active_pid, &trickle, TRICKLE_MSG, TR_IMIN,
                  TR_IDOUBLINGS, TR_REDCONST);

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case TRICKLE_MSG:
                trickle_callback((trickle_t *) msg.content.ptr);
                break;
            default:
                break;
        }
    }

    return 0;
}
