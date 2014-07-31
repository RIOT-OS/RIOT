/*
 * Copyright (C) 2013 Freie Universität Berlin
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
 * @brief       IPC pingpong test
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define LIMIT 1000

static char stacks[3][KERNEL_CONF_STACKSIZE_MAIN];
static int pids[3];

static void *first_thread(void *arg)
{
    (void) arg;

    puts("1st starting.");
    for (unsigned i = 0; i < LIMIT; ++i) {
        msg_t m;
        m.content.value = i;
        msg_send_receive(&m, &m, pids[1]);

        DEBUG("%u: Got msg with content %i\n", i, m.content.value);
        if (m.content.value != i + 1) {
            puts("ERROR. 1st");
            return NULL;
        }
    }
    puts("1st done.");

    return NULL;
}

static void *second_thread(void *arg)
{
    (void) arg;

    puts("2nd starting.");
    while (1) {
        msg_t m1;
        msg_receive(&m1);
        DEBUG("2nd: got msg from %i: %i\n", m1.sender_pid, m1.content.value);

        msg_t m2;
        m2.content.value = m1.content.value + 1;
        msg_send_receive(&m2, &m2, pids[2]);
        if (m2.content.value != m1.content.value) {
            puts("ERROR. 2nd");
            return NULL;
        }

        ++m1.content.value;
        msg_reply(&m1, &m1);
        if (m1.content.value == LIMIT) {
            break;
        }
    }
    puts("2nd done.");

    return NULL;
}

static void *third_thread(void *arg)
{
    (void) arg;

    puts("3rd starting.");
    while (1) {
        msg_t m;
        msg_receive(&m);
        DEBUG("3rd: got msg from %i: %i\n", m.sender_pid, m.content.value);

        --m.content.value;
        msg_reply(&m, &m);

        if (m.content.value == LIMIT - 1) {
            break;
        }
    }
    puts("3rd done.");

    return NULL;
}

int main(void)
{
    puts("Main thread start.");

    pids[0] = thread_create(stacks[0], sizeof(stacks[0]),
                            PRIORITY_MAIN - 1, CREATE_WOUT_YIELD | CREATE_STACKTEST,
                            first_thread, NULL, "1st");
    pids[1] = thread_create(stacks[1], sizeof(stacks[1]),
                            PRIORITY_MAIN - 2, CREATE_WOUT_YIELD | CREATE_STACKTEST,
                            second_thread, NULL, "2nd");
    pids[2] = thread_create(stacks[2], sizeof(stacks[2]),
                            PRIORITY_MAIN - 3, CREATE_WOUT_YIELD | CREATE_STACKTEST,
                            third_thread, NULL, "3nd");

    puts("Main thread done.");
    return 0;
}
