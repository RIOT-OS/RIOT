/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>
#include <string.h>

#include "extimer.h"
#include "msg.h"
#include "sched.h"
#include "xtimer.h"

#define TIME_MULTIPLIER (1000)
#define EVENT_TEST      (0x5555)
#define EVENT_NUM       (8)

msg_t msg_queue[EVENT_NUM];
extimer_event_t events[EVENT_NUM];
extimer_t timer;

static void reset(void)
{
    memset(events, 0, sizeof(events));
    extimer_init(&timer);
}

static void test_extimer_add(void)
{
    msg_t msg;
    uint32_t start = 0; /* will be overwritten but is here to silence pedantic
                         * compilers */
    int i;

    for (i = 0; i < EVENT_NUM; i++) {
        events[i].msg.type = EVENT_TEST;
        events[i].time = extimer_get_absolute(TIME_MULTIPLIER * i);
        if (i == 0) {
            start = xtimer_now();
        }
        extimer_add(&timer, &events[i], sched_active_pid);
        if (i != 0) {
            /* test duplicate detection for non-immediate events */
            extimer_add(&timer, &events[i], sched_active_pid);
        }
    }
    i = 0;
    while (xtimer_msg_receive_timeout(&msg,
                                      (EVENT_NUM + 1) * TIME_MULTIPLIER) >= 0) {
        if ((msg.type == EVENT_TEST)) {
            printf("event %i received after %u us\n", i++,
                   (unsigned)(xtimer_now() - start));
        }
    }
}

static void test_extimer_del(void)
{
    msg_t msg;
    uint32_t start = 0; /* will be overwritten but is here to silence pedantic
                         * compilers */
    int i;

    for (i = 0; i < EVENT_NUM; i++) {
        events[i].msg.type = EVENT_TEST;
        events[i].time = extimer_get_absolute(TIME_MULTIPLIER * i);
        if (i == 0) {
            start = xtimer_now();
        }
        extimer_add(&timer, &events[i], sched_active_pid);
    }
    i = 0;
    while (xtimer_msg_receive_timeout(&msg,
                                      (EVENT_NUM + 1) * TIME_MULTIPLIER) >= 0) {
        if (msg.type == EVENT_TEST) {
            printf("event %i received after %u us\n", i++,
                   (unsigned)(xtimer_now() - start));
            if (extimer_del(&timer, timer.events) != NULL) {
                i++;
            }
        }
    }
}

static void test_extimer_add_reverse(void)
{
    msg_t msg;
    uint32_t start = 0; /* will be overwritten but is here to silence pedantic
                         * compilers */
    int i;

    for (i = (EVENT_NUM - 1); i >= 0; i--) {
        events[i].msg.type = EVENT_TEST;
        events[i].time = extimer_get_absolute(TIME_MULTIPLIER * i);
        if (i == 0) {
            start = xtimer_now();
        }
        extimer_add(&timer, &events[i], sched_active_pid);
    }
    i = 0;
    while (xtimer_msg_receive_timeout(&msg,
                                      (EVENT_NUM + 1) * TIME_MULTIPLIER) >= 0) {
        if ((msg.type == EVENT_TEST)) {
            printf("event %i received after %u us\n", i++,
                   (unsigned)(xtimer_now() - start));
        }
    }
}

int main(void)
{
    msg_init_queue(msg_queue, EVENT_NUM);
    reset();
    puts("Test extimer_add()");
    test_extimer_add();
    reset();
    puts("Test extimer_add() (reversed order)");
    test_extimer_add_reverse();
    reset();
    puts("Test extimer_del()");
    test_extimer_del();
    puts("ALL TESTS SUCCESSFUL");
}

/** @} */
