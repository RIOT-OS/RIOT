/*
 * Copyright (C) 2017 INRIA
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
 * @brief    gnrc_mac timeout test application
 *
 * @author   Shuguo Zhuo <shuguo.zhuo@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "net/gnrc/mac/timeout.h"
#include "thread.h"
#include "msg.h"
#include "ztimer.h"

#define TIMEOUT_COUNT    3
#define TIMEOUT_1_DURATION    1000
#define TIMEOUT_2_DURATION    2538
#define TIMEOUT_3_DURATION    3471
static gnrc_mac_timeout_t mac_timeout;
static gnrc_mac_timeout_event_t test_timeouts[TIMEOUT_COUNT];
static gnrc_mac_timeout_type_t timeout_1;
static gnrc_mac_timeout_type_t timeout_2;
static gnrc_mac_timeout_type_t timeout_3;
static ztimer_now_t start_time;

static char worker_stack[THREAD_STACKSIZE_MAIN];

/* This thread will print the drift to stdout once per second */
void *worker_thread(void *arg)
{
    int count = 1;

    (void) arg;

    while (1) {
        msg_t m;
        ztimer_now_t now;

        msg_receive(&m);
        now = ztimer_now(ZTIMER_MSEC);

        if (gnrc_mac_timeout_is_expired(&mac_timeout, timeout_1)) {
            printf("At %6" PRIu32 " ms received msg %i: timeout_1 (set at %" PRIu32 " ms) expired, "
                   "supposed to be %" PRIu32 " ms!\n", now, count++, start_time, (TIMEOUT_1_DURATION + start_time));
        }

        if (gnrc_mac_timeout_is_expired(&mac_timeout, timeout_2)) {
            printf("At %6" PRIu32 " ms received msg %i: timeout_2 (set at %" PRIu32 " ms) expired, "
                   "supposed to be %" PRIu32 " ms!\n", now, count++, start_time, (TIMEOUT_2_DURATION + start_time));
        }

        if (gnrc_mac_timeout_is_expired(&mac_timeout, timeout_3)) {
            printf("At %6" PRIu32 " ms received msg %i: timeout_3 (set at %" PRIu32 " ms) expired, "
                   "supposed to be %" PRIu32 " ms!\n", now, count++, start_time, (TIMEOUT_3_DURATION + start_time));
        }

        if (gnrc_mac_timeout_is_running(&mac_timeout, timeout_1)) {
            printf("At %6" PRIu32 " ms: timeout_1 is running.\n", now);
        }
        else {
            printf("At %6" PRIu32 " ms: timeout_1 is not running.\n", now);
        }

        if (gnrc_mac_timeout_is_running(&mac_timeout, timeout_2)) {
            printf("At %6" PRIu32 " ms: timeout_2 is running.\n", now);
        }
        else {
            printf("At %6" PRIu32 " ms: timeout_2 is not running.\n", now);
        }

        if (gnrc_mac_timeout_is_running(&mac_timeout, timeout_3)) {
            printf("At %6" PRIu32 " ms: timeout_3 is running.\n", now);
        }
        else {
            printf("At %6" PRIu32 " ms: timeout_3 is not running.\n", now);
        }
    }
}

int main(void)
{
    /* create worker thread */
    kernel_pid_t pid = thread_create(worker_stack, sizeof(worker_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     0,
                                     worker_thread, NULL, "worker");

    timeout_1 = -1;
    timeout_2 = -2;
    timeout_3 = -3;

    start_time = ztimer_now(ZTIMER_MSEC);
    gnrc_mac_init_timeouts(&mac_timeout, test_timeouts, TIMEOUT_COUNT);
    gnrc_mac_set_timeout(&mac_timeout, timeout_1, TIMEOUT_1_DURATION, pid);
    gnrc_mac_set_timeout(&mac_timeout, timeout_2, TIMEOUT_2_DURATION, pid);
    gnrc_mac_set_timeout(&mac_timeout, timeout_3, TIMEOUT_3_DURATION, pid);
    printf("Testing gnrc_mac timeout module (start time = %" PRIu32 " ms)\n", start_time);
    printf("Set timeout_1, should be expired at %" PRIu32 " ms)\n", TIMEOUT_1_DURATION + start_time);
    printf("Set timeout_2, should be expired at %" PRIu32 " ms)\n", TIMEOUT_2_DURATION + start_time);
    printf("Set timeout_3, should be expired at %" PRIu32 " ms)\n", TIMEOUT_3_DURATION + start_time);

    puts("Are the reception times of all 3 msgs close to the supposed values?\n");
    puts("If yes, the tests were successful");
}
