/*
 * Copyright (C) 2015-19 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 INRIA
 *               2017 HAW Hamburg
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
 * @brief       ztimer64_msg test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <stdio.h>

#include "ztimer64.h"
#include "thread.h"
#include "msg.h"
#include "timex.h"

#include "test_utils/expect.h"

#ifdef MODULE_ZTIMER64_SEC
#define ZTIMER64 ZTIMER64_SEC
#define TICKS_PER_SEC 1
#elif MODULE_ZTIMER64_MSEC
#define ZTIMER64 ZTIMER64_MSEC
#define TICKS_PER_SEC MS_PER_SEC
#else
#define ZTIMER64 ZTIMER64_USEC
#define TICKS_PER_SEC US_PER_SEC
#endif

char timer_stack[THREAD_STACKSIZE_DEFAULT];
char timer_stack_local[THREAD_STACKSIZE_DEFAULT];

struct timer_msg {
    ztimer64_t timer;
    uint64_t interval;
    char *text;
    msg_t msg;
};

struct timer_msg msg_a = { .interval = (2 * TICKS_PER_SEC),
                           .text = "Hello World" };
struct timer_msg msg_b = { .interval = (5 * TICKS_PER_SEC),
                           .text = "This is a Test" };

void *timer_thread(void *arg)
{
    (void)arg;

    printf("This is thread %" PRIkernel_pid "\n", thread_getpid());

    /* The queue is required to avoid loss of a 2nd message, when the 1st is
     * still processed. The timing ensures that at most 1 message is queued.
     */
    msg_t msgq[1];

    msg_init_queue(msgq, 1);

    while (1) {
        msg_t m;
        msg_receive(&m);
        struct timer_msg *tmsg = m.content.ptr;
        uint64_t now = ztimer64_now(ZTIMER64);
        /* casts are needed to solve for sometimes TICKS_PER_SEC being UL
         * result of / and % of uint32_t will always fit into uint32_t
         */
        printf(
            "now=%" PRIu32 ":%" PRIu32 " -> every %" PRIu32 ".%" PRIu32 "s: %s\n",
            (uint32_t)(now / TICKS_PER_SEC),
            (uint32_t)(now % TICKS_PER_SEC),
            (uint32_t)(tmsg->interval / TICKS_PER_SEC),
            (uint32_t)(tmsg->interval % TICKS_PER_SEC),
            tmsg->text);

        tmsg->msg.type = 12345;
        tmsg->msg.content.ptr = tmsg;
        ztimer64_set_msg(ZTIMER64, &tmsg->timer, tmsg->interval, &tmsg->msg,
                         thread_getpid());
    }
}

void *timer_thread_local(void *arg)
{
    (void)arg;

    printf("This is thread %" PRIkernel_pid "\n", thread_getpid());

    while (1) {
        msg_t m;
        msg_receive(&m);

        uint64_t now = ztimer64_now(ZTIMER64);
        uint32_t sec = now / TICKS_PER_SEC;
        uint32_t min = sec / 60;
        uint32_t hr = sec / 3600;
        printf("sec=%" PRIu32 " min=%" PRIu32 " hour=%" PRIu32 "\n", sec, min,
               hr);
    }
}

int main(void)
{
    msg_t m;
    kernel_pid_t pid = thread_create(
        timer_stack,
        sizeof(timer_stack),
        THREAD_PRIORITY_MAIN - 1,
        0,
        timer_thread,
        NULL,
        "timer");

    expect(pid_is_valid(pid));

    puts("sending 1st msg");
    m.content.ptr = &msg_a;
    msg_try_send(&m, pid);

    puts("sending 2nd msg");
    m.content.ptr = &msg_b;
    msg_try_send(&m, pid);

    kernel_pid_t pid2 = thread_create(
        timer_stack_local,
        sizeof(timer_stack_local),
        THREAD_PRIORITY_MAIN - 1,
        0,
        timer_thread_local,
        NULL,
        "timer local");

    expect(pid_is_valid(pid2));

    while (1) {
        ztimer64_sleep(ZTIMER64, 1LLU * TICKS_PER_SEC);
        msg_try_send(&m, pid2);
    }
}
