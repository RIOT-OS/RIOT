/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 INRIA
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
 * @brief xtimer_msg test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <time.h>

#include "xtimer.h"
#include "thread.h"
#include "msg.h"

char timer_stack[THREAD_STACKSIZE_MAIN];
char timer_stack_local[THREAD_STACKSIZE_MAIN];

struct timer_msg {
    xtimer_t timer;
    uint32_t interval;
    char *text;
    msg_t msg;
};

struct timer_msg msg_a = { .interval = 2*(1000000), .text = "Hello World", };
struct timer_msg msg_b = { .interval = 5*(1000000), .text = "This is a Test" };

void *timer_thread(void *arg)
{
    (void) arg;
    timex_t now;

    printf("This is thread %" PRIkernel_pid "\n", thread_getpid());

    /* we need a queue if the second message arrives while the first is still processed */
    /* without a queue, the message would get lost */
    /* because of the way this timer works, there can be max 1 queued message */
    msg_t msgq[1];
    msg_init_queue(msgq, 1);

    while (1) {
        msg_t m;
        msg_receive(&m);
        struct timer_msg *tmsg = (struct timer_msg *) m.content.ptr;
        xtimer_now_timex(&now);
        printf("now=%" PRIu32 ":%" PRIu32 " -> every %" PRIu32 ".%" PRIu32 "s: %s\n",
               now.seconds,
               now.microseconds,
               tmsg->interval / 1000000,
               tmsg->interval % 1000000,
               tmsg->text);

        tmsg->msg.type = 12345;
        tmsg->msg.content.ptr = (void*)tmsg;
        xtimer_set_msg(&tmsg->timer, tmsg->interval, &tmsg->msg, thread_getpid());
    }
}

void *timer_thread_local(void *arg)
{
    (void) arg;

    printf("This is thread %" PRIkernel_pid "\n", thread_getpid());

    while (1) {
        msg_t m;
        msg_receive(&m);

        uint32_t now = xtimer_now();
        int sec, min, hr;
        sec = now/1000000;
        min = sec/60;
        hr = sec/3600;
        printf("sec=%d min=%d hour=%d\n", sec, min, hr);
    }
}

int main(void)
{
    msg_t m;
    kernel_pid_t pid = thread_create(
                  timer_stack,
                  sizeof(timer_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  timer_thread,
                  NULL,
                  "timer");

    puts("sending 1st msg");
    m.content.ptr = (char *) &msg_a;
    msg_try_send(&m, pid);

    puts("sending 2nd msg");
    m.content.ptr = (char *) &msg_b;
    msg_try_send(&m, pid);

    kernel_pid_t pid2 = thread_create(
                   timer_stack_local,
                   sizeof(timer_stack_local),
                   THREAD_PRIORITY_MAIN - 1,
                   THREAD_CREATE_STACKTEST,
                   timer_thread_local,
                   NULL,
                   "timer local");

    while (1) {
        xtimer_usleep(1*1000000);
        msg_try_send(&m, pid2);
    }
}
