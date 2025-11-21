/*
 * SPDX-FileCopyrightText: 2015 INRIA
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       test application for xtimer_msg_receive_timeout()
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */
#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "xtimer.h"
#include "timex.h"

#define TEST_PERIOD     (100LU * US_PER_MS) /* 100ms in US */
#define TEST_COUNT      (10LU)

int main(void)
{
    msg_t m, tmsg;
    xtimer_t t;
    int64_t offset = -(TEST_PERIOD/10);
    tmsg.type = 42;
    puts("[START]");
    for (unsigned i = 0; i < TEST_COUNT; i++) {
        xtimer_set_msg(&t, TEST_PERIOD + offset, &tmsg, thread_getpid());
        if (xtimer_msg_receive_timeout(&m, TEST_PERIOD) < 0) {
            puts("Timeout!");
            msg_receive(&m);
        }
        else {
            printf("Message: %" PRIu16 "\n", m.type);
        }
        /* flip sign */
        offset *= (-1);
    }
    puts("[SUCCESS]");
    return 0;
}
