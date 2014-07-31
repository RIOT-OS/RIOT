/*
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2014 Kaspar Schleiser
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
 * @brief Thread test application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <time.h>

#include "vtimer.h"
#include "thread.h"
#include "msg.h"

#define MAXCOUNT 100
#define MAXDIFF 10000

char timer_stack[KERNEL_CONF_STACKSIZE_MAIN*4];

struct timer_msg {
    vtimer_t timer;
    timex_t interval;
    char *msg;
    timex_t start;
    int count;
};

timex_t now;

struct timer_msg timer_msgs[] = { { .interval = { .seconds = 0, .microseconds = 100000}, .msg = "T1", .start={0}, .count=0 },
                                  { .interval = { .seconds = 0, .microseconds = 200000}, .msg = "T2", .start={0}, .count=0 },
                                  { .interval = { .seconds = 0, .microseconds = 300000}, .msg = "T3", .start={0}, .count=0 },
                                  { .interval = { .seconds = 0, .microseconds = 500000}, .msg = "T4", .start={0}, .count=0 },
                                  { .interval = { .seconds = 0, .microseconds = 700000}, .msg = "T5", .start={0}, .count=0 },
                                  { .interval = { .seconds = 1, .microseconds = 100000}, .msg = "T6", .start={0}, .count=0 },
};

void *timer_thread(void *arg)
{
    (void) arg;
    printf("This is thread %d\n", thread_getpid());

    msg_t msgq[16];
    msg_init_queue(msgq, sizeof(msgq));

    while (1) {
        msg_t m;
        msg_receive(&m);
        struct timer_msg *tmsg = (struct timer_msg *) m.content.ptr;

        vtimer_now(&now);

        if (tmsg->start.seconds==0 && tmsg->start.microseconds == 0) {
            printf("Initializing \"%s\".\n", tmsg->msg);
            tmsg->start = now;
        } else {
            tmsg->count++;
        }

        uint64_t should = timex_uint64(tmsg->interval)*tmsg->count+timex_uint64(tmsg->start);
        int64_t diff = should - timex_uint64(now);

        printf("now=%02" PRIu32 ":%02" PRIu32 " -> every %" PRIu32 ".%" PRIu32 "s: %s diff=%" PRId64 "\n",
               now.seconds,
               now.microseconds,
               tmsg->interval.seconds,
               tmsg->interval.microseconds,
               tmsg->msg, diff);

        if (diff > MAXDIFF || diff < -MAXDIFF) {
            printf("WARNING: timer difference %" PRId64 "us exceeds MAXDIFF(%d)!\n", diff, MAXDIFF);
        }

        if (vtimer_set_msg(&tmsg->timer, tmsg->interval, thread_getpid(), tmsg) != 0) {
            puts("something went wrong setting a timer");
        }

        if (tmsg->count >= MAXCOUNT) {
            printf("Maximum count reached. (%d) Exiting.\n", MAXCOUNT);
            break;
        }
    }

    return NULL;
}

int main(void)
{
    msg_t m;
    int pid = thread_create(
                  timer_stack,
                  sizeof(timer_stack),
                  PRIORITY_MAIN - 1,
                  CREATE_STACKTEST,
                  timer_thread,
                  NULL,
                  "timer");

    for (unsigned i = 0; i < sizeof(timer_msgs)/sizeof(struct timer_msg); i++) {
        printf("Sending timer msg %u...\n", i);
        m.content.ptr = (char *) &timer_msgs[i];
        msg_send(&m, pid, false);
    }

    return 0;
}
