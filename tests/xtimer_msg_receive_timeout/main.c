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
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "thread.h"
#include "msg.h"
#include "xtimer.h"
#include "timex.h"

int main(void)
{
    msg_t m, tmsg;
    xtimer_t t;
    int64_t offset = -1000;
    tmsg.type = 44;

    for (int i = 0; i < 10; i++) {
        xtimer_set_msg(&t, SEC_IN_USEC + offset, &tmsg, sched_active_pid);
        if (xtimer_msg_receive_timeout(&m, SEC_IN_USEC) < 0) {
            puts("Timeout!");
        }
        else {
            printf("Message received: %" PRIu16 "\n", m.type);
        }
        offset = (offset < 0) ? 1000 : -1000;
    }
    return 0;
}
