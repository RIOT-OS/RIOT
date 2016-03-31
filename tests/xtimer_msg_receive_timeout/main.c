/*
 * Copyright (C) 2015 INRIA
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
 * @brief       test application for xtimer_msg_receive_timeout()
 *
 *              This test will start sequentially start 10 xtimers to send a
 *              IPC msg, alternating with an interval of 900ms and 1100ms
 *              respectively. Everytime a timer was set, it will wait for a
 *              message for at most 1000ms. This should succeed and fail in an
 *              alternating manner.
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "thread.h"
#include "msg.h"
#include "xtimer.h"
#include "timex.h"

#define TEST_PERIOD (100000LU)

int main(void)
{
    msg_t m, tmsg;
    xtimer_t t;
    int64_t offset = -1000;
    tmsg.type = 44;
    t.target = 0;
    t.long_target = 0;

    for (int i = 0; i < 10; i++) {
        xtimer_set_msg(&t, TEST_PERIOD + offset, &tmsg, sched_active_pid);
        if (xtimer_msg_receive_timeout(&m, TEST_PERIOD) < 0) {
            puts("Timeout!");
        }
        else {
            printf("Message received: %" PRIu16 "\n", m.type);
        }
        offset = (offset < 0) ? 1000 : -1000;
        xtimer_remove(&t);
    }
    return 0;
}
