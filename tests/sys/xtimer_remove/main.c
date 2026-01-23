/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       timer test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "thread.h"
#include "xtimer.h"
#include "test_utils/expect.h"

#define NUMOF (3U)

int main(void)
{
    puts("xtimer_remove test application.\n");

    kernel_pid_t me = thread_getpid();

    for (unsigned int n = 0; n < NUMOF; n++) {
        printf("Setting %u timers, removing timer %u/%u\n", NUMOF, n, NUMOF);
        xtimer_t timers[NUMOF];
        msg_t msg[NUMOF];
        for (unsigned int i = 0; i < NUMOF; i++) {
            msg[i].type = i;
            xtimer_set_msg(&timers[i], 100000*(i+1), &msg[i], me);
            expect(xtimer_is_set(&timers[i]));
        }

        xtimer_remove(&timers[n]);
        expect(!xtimer_is_set(&timers[n]));

        unsigned int num = NUMOF-1;
        while(num--) {
            msg_t m;
            msg_receive(&m);
            if (m.type == n) {
                printf("ERROR: msg type=%i unexpected!\n", m.type);
                return -1;
            }
            else {
                printf("timer %u triggered.\n", m.type);
            }
        }
    }

    printf("test successful.\n");

    return 0;
}
