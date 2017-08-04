/*
 * Copyright (C) 2017 Technische Universität Berlin
 *               2017 Freie Universität Berlin
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
 * @brief       Test application for testing priority inheritance for nested
 *              mutexes
 *
 * @author      Thomas Geithner <thomas.geithner@dai-labor.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "thread.h"
#include "msg.h"
#include "xtimer.h"

#define TICK_LEN            (50 * US_PER_MS)
#define EXSPECTED_RESULT    (-3)

#define T_NUMOF             (3U)
#define MSG_TYPE            (0xabcd)

static char stacks[T_NUMOF][THREAD_STACKSIZE_MAIN];
static kernel_pid_t pids[T_NUMOF];
static const char *names[] = { "t1", "t2", "t3" };

static int result = 0;
static int res_addsub = 1;

static inline void delay(unsigned ticks)
{
    xtimer_usleep(ticks * TICK_LEN);
}

static inline void event(int num, const char *t_name, const char *msg)
{
    /* record event */
    result += (res_addsub * num);
    res_addsub *= -1;

    printf("Event %2i: %7s - %s\n", num, t_name, msg);
}

static void *t1(void *arg)
{
    (void)arg;
    msg_t m;
    msg_t rply;

    m.type = MSG_TYPE;
    m.content.value = (uint32_t)'M';

    delay(2);

    event(3, "t1", "sending msg to t3 (msg_send_receive)");
    msg_send_receive(&m, &rply, pids[2]);
    event(6, "t1", "received reply");

    return NULL;
}

static void *t2(void *arg)
{
    (void)arg;

    delay(1);

    event(2, "t2", "starting infinite loop, potentially starving others");
    while (1) {
        thread_yield_higher();
    }

    return NULL;
}

static void *t3(void *arg)
{
    (void)arg;
    msg_t m;
    msg_t rply;

    rply.type = MSG_TYPE;
    rply.content.value = (uint32_t)'m';

    event(1, "t3", "waiting for incoming message");
    msg_receive(&m);
    event(4, "t3", "received message");

    event(5, "t3", "sending reply");
    msg_reply(&m, &rply);

    return NULL;
}

static thread_task_func_t handlers[] = { t1, t2, t3 };

int main(void)
{
    puts("Test for showing priority inversion when using msg_send_receive\n");
    puts("If this tests succeeds, you should see 6 events appearing in order.\n"
         "The expected output should look like this:\n"
         "Event  1:      t3 - waiting for incoming message\n"
         "Event  2:      t2 - starting infinite loop, potentially starving others\n"
         "Event  3:      t1 - sending msg to t3 (msg_send_receive)\n"
         "Event  4:      t3 - received message\n"
         "Event  5:      t3 - sending reply\n"
         "Event  6:      t1 - received reply\n");
    puts("TEST OUTPUT:");

    /* create threads */
    for (unsigned i = 0; i < T_NUMOF; i++) {
        pids[i] = thread_create(stacks[i], sizeof(stacks[i]),
                                (THREAD_PRIORITY_MAIN + 1 + i),
                                THREAD_CREATE_WOUT_YIELD,
                                handlers[i], NULL,
                                names[i]);
    }

    delay(3);

    if (result == EXSPECTED_RESULT) {
        puts("\n   *** result: SUCCESS ***");
    }
    else {
        puts("\n   *** result: FAILED ***");
    }

    return 0;
}
