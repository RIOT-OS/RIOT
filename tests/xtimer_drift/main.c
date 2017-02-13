/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2015 Eistec AB
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
 * @brief    xtimer_drift test application
 *
 * @author   Kaspar Schleiser <kaspar@schleiser.de>
 * @author   Oliver Hahm <oliver.hahm@inria.fr>
 * @author   Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author   Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>
#include <time.h>

#include "xtimer.h"
#include "thread.h"
#include "msg.h"

/* We generate some context switching and IPC traffic by using multiple threads
 * and generate some xtimer load by scheduling several messages to be called at
 * different times. TEST_HZ is the frequency of messages being sent from the
 * main thread to the worker, all other message frequencies are derived from
 * TEST_HZ.
 * TEST_MSG_RX_USLEEP is a tiny sleep inside the message reception thread to
 * cause extra context switches.
 */
#define TEST_HZ (64LU)
#define TEST_INTERVAL (1000000LU / TEST_HZ)
#define TEST_MSG_RX_USLEEP (200LU)

char slacker_stack1[THREAD_STACKSIZE_DEFAULT];
char slacker_stack2[THREAD_STACKSIZE_DEFAULT];
char worker_stack[THREAD_STACKSIZE_MAIN];

struct timer_msg {
    xtimer_t timer;
    uint32_t interval;
    msg_t msg;
};

struct timer_msg msg_a = { .interval = (TEST_INTERVAL / 2) };
struct timer_msg msg_b = { .interval = (TEST_INTERVAL / 3) };
struct timer_msg msg_c = { .interval = (TEST_INTERVAL * 5) };
struct timer_msg msg_d = { .interval = (TEST_INTERVAL * 2) };

/* This thread is only here to give the kernel some extra load */
void *slacker_thread(void *arg)
{
    (void) arg;
    timex_t now;

    printf("Starting thread %" PRIkernel_pid "\n", thread_getpid());

    /* we need a queue if the second message arrives while the first is still processed */
    /* without a queue, the message would get lost */
    msg_t msgq[4];
    msg_init_queue(msgq, 4);

    while (1) {
        msg_t m;
        msg_receive(&m);
        struct timer_msg *tmsg = m.content.ptr;
        xtimer_now_timex(&now);
        xtimer_usleep(TEST_MSG_RX_USLEEP);

        tmsg->msg.type = 12345;
        tmsg->msg.content.ptr = tmsg;
        xtimer_set_msg(&tmsg->timer, tmsg->interval, &tmsg->msg, thread_getpid());
    }
}

/* This thread will print the drift to stdout once per second */
void *worker_thread(void *arg)
{
    (void) arg;
    uint32_t loop_counter = 0;
    uint32_t start = 0;
    uint32_t last = 0;

    printf("Starting thread %" PRIkernel_pid "\n", thread_getpid());

    while (1) {
        msg_t m;
        msg_receive(&m);
        xtimer_ticks32_t ticks = xtimer_now();
        uint32_t now = xtimer_usec_from_ticks(ticks);
        if (start == 0) {
            start = now;
            last = start;
            ++loop_counter;
            continue;
        }

        uint32_t us, sec;
        us = now % US_PER_SEC;
        sec = now / US_PER_SEC;
        if ((loop_counter % TEST_HZ) == 0) {
            uint32_t expected = start + loop_counter * TEST_INTERVAL;
            int32_t drift = now - expected;
            expected = last + TEST_HZ * TEST_INTERVAL;
            int32_t jitter = now - expected;
            printf("now=%" PRIu32 ".%06" PRIu32 " (0x%08" PRIx32 " ticks), ",
                sec, us, ticks.ticks32);
            printf("drift=%" PRId32 " us, jitter=%" PRId32 " us\n", drift, jitter);
            last = now;
        }
        ++loop_counter;
    }
}

int main(void)
{
    msg_t m;

    puts("xtimer_drift test application");
    puts("Make note of the PC clock when starting this test, let run for a while, "
        "compare the printed time against the expected time from the PC clock.");
    puts("The difference is the RIOT timer drift, this is likely caused by either: "
        "an inaccurate hardware timer, or bugs in the software (xtimer or periph/timer).");
    printf("This test will run a periodic timer every %lu microseconds (%lu Hz), ",
        (unsigned long)TEST_INTERVAL, (unsigned long)TEST_HZ);
    puts("The current time will be printed once per second, along with the "
         "difference between the actual and expected xtimer_now value.");
    puts("The first output variable, 'drift', represents the total offset since "
         "start between xtimer_now and the expected time.");
    puts("The second output variable, 'jitter', represents the difference in drift from the last printout.");
    puts("Two other threads are also running only to cause extra interrupts and context switches.");
    puts(" <====== PC clock if running in pyterm.");
    puts("");
    puts(" =======================");
    puts(" ===== Test begins =====");
    puts(" =======================");

    kernel_pid_t pid1 = thread_create(
        slacker_stack1,
        sizeof(slacker_stack1),
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_STACKTEST,
        slacker_thread,
        NULL,
        "slacker1");

    puts("sending 1st msg");
    m.content.ptr = &msg_a;
    msg_try_send(&m, pid1);

    puts("sending 2nd msg");
    m.content.ptr = &msg_b;
    msg_try_send(&m, pid1);

    kernel_pid_t pid2 = thread_create(
        slacker_stack2,
        sizeof(slacker_stack2),
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_STACKTEST,
        slacker_thread,
        NULL,
        "slacker2");

    puts("sending 3rd msg");
    m.content.ptr = &msg_c;
    msg_try_send(&m, pid2);

    puts("sending 4th msg");
    m.content.ptr = &msg_d;
    msg_try_send(&m, pid2);

    kernel_pid_t pid3 = thread_create(
                   worker_stack,
                   sizeof(worker_stack),
                   THREAD_PRIORITY_MAIN - 1,
                   THREAD_CREATE_STACKTEST,
                   worker_thread,
                   NULL,
                   "worker");

    xtimer_ticks32_t last_wakeup = xtimer_now();
    while (1) {
        xtimer_periodic_wakeup(&last_wakeup, TEST_INTERVAL);
        msg_try_send(&m, pid3);
    }
}
