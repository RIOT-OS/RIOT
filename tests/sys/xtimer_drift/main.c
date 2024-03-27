/*
 * Copyright (C) 2017 HAW Hamburg
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
 * @author   Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include <time.h>

#include "xtimer.h"
#include "thread.h"
#include "msg.h"
#include "log.h"

/* We generate some context switching and IPC traffic by using multiple threads
 * and generate some xtimer load by scheduling several messages to be called at
 * different times. TEST_HZ is the frequency of messages being sent from the
 * main thread to the worker, all other message frequencies are derived from
 * TEST_HZ.
 * TEST_MSG_RX_USLEEP is a tiny sleep inside the message reception thread to
 * cause extra context switches.
 */
#define TEST_HZ             (64LU)
#define TEST_INTERVAL       (1000000LU / TEST_HZ)
#define TEST_MSG_RX_USLEEP  (200LU)
#define TEST_MSG_QUEUE_SIZE (4U)

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

    LOG_DEBUG("run thread %" PRIkernel_pid "\n", thread_getpid());

    /* we need a queue if a 2nd message arrives while the first is processed */
    msg_t msgq[TEST_MSG_QUEUE_SIZE];
    msg_init_queue(msgq, TEST_MSG_QUEUE_SIZE);

    while (1) {
        msg_t m;
        msg_receive(&m);
        struct timer_msg *tmsg = m.content.ptr;
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

    /* Calculate interval based on possible precision when 'XTIMER_SHIFT > 0',
     * to apply precision loss to expected interval length.
     * test_interval != TEST_INTERVAL */
    uint32_t test_interval = xtimer_usec_from_ticks(xtimer_ticks_from_usec(TEST_INTERVAL));
    uint32_t loop_counter = 0;
    uint32_t start = 0;
    uint32_t last = 0;

    LOG_DEBUG("run thread %" PRIkernel_pid "\n", thread_getpid());

    while (1) {
        msg_t m;
        msg_receive(&m);

        xtimer_ticks32_t ticks = xtimer_now();
        uint32_t now = xtimer_usec_from_ticks(ticks);

        if (start == 0) {
            start = now;
            last = start;
        }
        else if ((loop_counter % TEST_HZ) == 0) {
            uint32_t us = now % US_PER_SEC;
            uint32_t sec = now / US_PER_SEC;
            uint32_t expected = start + loop_counter * test_interval;
            int32_t drift = now - expected;
            expected = last + TEST_HZ * test_interval;
            int32_t jitter = now - expected;
            printf("now=%" PRIu32 ".%06" PRIu32 " (0x%08" PRIx32 " ticks), ",
#ifdef MODULE_ZTIMER_XTIMER_COMPAT
                   sec, us, ticks);
#else
                   sec, us, ticks.ticks32);
#endif
            printf("drift=%" PRId32 " us, jitter=%" PRId32 " us\n",
                   drift, jitter);
            last = now;
        }
        ++loop_counter;
    }
}

int main(void)
{
    LOG_DEBUG("[INIT]\n");
    msg_t m;
    /* create and trigger first background thread */
    kernel_pid_t pid1 = thread_create(slacker_stack1, sizeof(slacker_stack1),
                                      THREAD_PRIORITY_MAIN - 1,
                                      0,
                                      slacker_thread, NULL, "slacker1");

    LOG_DEBUG("+ msg 1");
    m.content.ptr = &msg_a;
    msg_try_send(&m, pid1);

    LOG_DEBUG("+ msg 2");
    m.content.ptr = &msg_b;
    msg_try_send(&m, pid1);

    /* create and trigger second background thread */
    kernel_pid_t pid2 = thread_create(slacker_stack2, sizeof(slacker_stack2),
                                      THREAD_PRIORITY_MAIN - 1,
                                      0,
                                      slacker_thread, NULL, "slacker2");

    LOG_DEBUG("+ msg 3");
    m.content.ptr = &msg_c;
    msg_try_send(&m, pid2);

    LOG_DEBUG("+ msg 4");
    m.content.ptr = &msg_d;
    msg_try_send(&m, pid2);

    /* create and trigger worker thread */
    kernel_pid_t pid3 = thread_create(worker_stack, sizeof(worker_stack),
                                      THREAD_PRIORITY_MAIN - 2,
                                      0,
                                      worker_thread, NULL, "worker");

    puts("[START]\n");
    xtimer_ticks32_t last_wakeup = xtimer_now();
    while (1) {
        xtimer_periodic_wakeup(&last_wakeup, TEST_INTERVAL);
        msg_try_send(&m, pid3);
    }
}
